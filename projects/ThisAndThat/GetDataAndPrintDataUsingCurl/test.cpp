#include <curl/curl.h>
#include <stdio.h>

#include <stdlib.h>
#include <windows.h>
#include <iostream>

#define INITIAL_MEMORY_SIZE 4
#define TMP_ERROR_RETURN 1
#define ERROR_MESSAGE_BUFFER 40
#define ERROR_MEMORY_SIZE 0xffffffff
#define ERROR_CHECK_AND_MESSAGE_AND_RETURN(X,Y) printf( "Line %d -> ", __LINE__ );                      \
                                                printf("%s failed: % s\n", X, curl_easy_strerror(Y) ); \
                                                curl_easy_cleanup(pCtx);                               \
                                                                                                       \
                                                nReturnCode = (int)cErrorCode;                         \
                                                goto memoryrelease;                                \


struct DownloadFileData {
    char* memory;
    size_t size;
};


//*  CURL에서 사용할 WriteFunction
//*
//*  @param  contents		   libcurl이 수신한 데이터
//*  @param  size              데이터 블록의 크기,대부분의 경우 이 값은 1
//*  @param  nmemb	           contents에 전달된 데이터의 개수
//*  @param  userp             사용자 정의 데이터를 가리키는 포인터
//*
//*  @retval realsize		   수신한 최종 데이터
//*  @retval ERROR_MEMORY_SIZE 실패   
//*
//*  @remark
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {

    size_t realsize = 0;

    if (NULL == contents || NULL == userp) {
        realsize = ERROR_MEMORY_SIZE;

        return realsize;
    }

    // 1. 파일의 실제 사이즈 계산
    realsize = size * nmemb;
    struct DownloadFileData* mem = (struct DownloadFileData*)userp;

    // 2. 파일의 크기에 맞게 메모리 재할당
    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        realsize = ERROR_MEMORY_SIZE;

        goto memoryrelease;
    }

    // 3. 재할당한 메모리에 다운로드 받은 파일 담기
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

memoryrelease:
    return realsize;
}


//*  파일을 다운로드하기 위해, context 객체에 옵션을 지정하고
//*  다운로드 받고, context 객체를 초기화 하는 함수
//*
//*  @param  pCtx			   context 객체
//*  @param  pszUrL            통신할 서버의 URL
//*  @param  pszResultFileName 다운로드 받아서 저장될 파일의 경로
//*
//*  @retval FUNCTION_SUCCESS  성공
//*  @retval 나머지 에러코드 
//*
//*  @remark
int DownloadFile(CURL* pCtx, LPSTR pszUrL, LPSTR pszResultFileName) {

    int nReturnCode = 0;
    CURLcode cErrorCode = CURLE_OK;
    int nIsSuccess = 0;
    DWORD dwBytesWritten = 0;
    HANDLE hFile = NULL;

    if (pCtx == NULL || NULL == pszUrL || NULL == pszResultFileName) {
        nReturnCode = 1;
        goto memoryrelease;
    }

    struct DownloadFileData filedata;
    filedata.memory = (char*)malloc(INITIAL_MEMORY_SIZE); //
    filedata.size = 0;


    // 1. context 객체 옵션 지정
    // 1-1. URL
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_URL, pszUrL);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 1-2. HOSTNAME 검증 옵션
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_SSL_VERIFYHOST, 1L);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 1-3. 인증서 검증 옵션
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_SSL_VERIFYPEER, 2L);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 1-4. progress bar
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_NOPROGRESS, 0L);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 1-5. WriteFunction
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 1-6. 사용자 정의 데이터
    cErrorCode = curl_easy_setopt(pCtx, CURLOPT_WRITEDATA, (void*)&filedata);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_setopt()", cErrorCode)
    }

    // 2. 파일 다운로드
    cErrorCode = curl_easy_perform(pCtx);
    if (CURLE_OK != cErrorCode) {
        ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_perform()", cErrorCode)
    }

    else {
        // 3. HTTP Code 확인
        long lHttpCode = 0;

        cErrorCode = curl_easy_getinfo(pCtx, CURLINFO_HTTP_CODE, &lHttpCode);
        switch (cErrorCode) {
        case CURLE_OK:
            printf("\n\nResponse code : %d\n", lHttpCode);
            break;
        default:
            ERROR_CHECK_AND_MESSAGE_AND_RETURN("curl_easy_perform()", cErrorCode)
        }

        // 4. HTTP Code가 200이면 성공
        if (200 == lHttpCode) {
            printf("Function DownloadFile -> pszResultFileName : %s\n", pszResultFileName);
            hFile = CreateFileA((LPSTR)pszResultFileName, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL, NULL);

            // 4-1. 다운로드 받은 파일을 원하는 로컬 파일에 저장 
            if (INVALID_HANDLE_VALUE != hFile) {
                nIsSuccess = WriteFile(hFile, filedata.memory, filedata.size, &dwBytesWritten, NULL);
                if (0 == nIsSuccess) {
                    nReturnCode = GetLastError();
                    printf("Line %d : ", __LINE__);

                    goto memoryrelease;
                }
                // 파일 생성 성공
                else {
                    printf("\nFile Download Successfully!\n");
                }
            }
            else {
                nReturnCode = GetLastError();
                printf("Line %d : ", __LINE__);

                goto memoryrelease;
            }
        }

        // 5. HTTP Code가 200이 아님 - 다운로드 실패
        else {
            printf("File Download Fail!\nHTTP Code : %d\n", lHttpCode);
        }
    }

memoryrelease:

    //curl context 객체 reset
    curl_easy_reset(pCtx);

    if (INVALID_HANDLE_VALUE != hFile) {
        CloseHandle(hFile);
    }

    printf("\n\n=====================\n\n");
    return nReturnCode;
}

int main() {
    // Variables
    CURL* pCurlHandle = NULL;
    char szURL[MAX_PATH] = "http://apihub.kma.go.kr/api/typ01/url/upp_temp.php?tm=201806210000&stn=0&pa=0&help=1&authKey=Y18myu4eT22fJsruHn9tyA";
    char szResultFile[MAX_PATH] = "output.txt";
    char szThreeLine[MAX_PATH] = "\n\n\n";
    HANDLE hFile = NULL;
    DWORD dw = 0;

    // 1. curl 초기화
    curl_global_init(CURL_GLOBAL_ALL);
    pCurlHandle = curl_easy_init();

    // 2. 파일 다운로드
    for (int i = 0; i < 2; i++) {
        DownloadFile(pCurlHandle, szURL, szResultFile);
        hFile = CreateFileA(szResultFile, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        WriteFile(hFile, szThreeLine, sizeof(szThreeLine), &dw, NULL);
        CloseHandle(hFile);
        hFile = NULL;
    }

    // 3. finalize
    if (hFile != NULL) CloseHandle(hFile);
    curl_easy_cleanup(pCurlHandle);
    curl_global_cleanup();
    
    return 0;
}