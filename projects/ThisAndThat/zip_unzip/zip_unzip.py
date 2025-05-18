import os
import zipfile

#첫 압축
def create_zip_with_tmp(input_path: str, n: int):
    tmp_name = f"tmp{n}.txt"
    zip_name = f"zip{n}.zip"

    # 1) tmp1.txt 생성 및 내용 쓰기
    with open(tmp_name, "w", encoding="utf-8") as f:
        f.write(str(n))
        
    # 2) zip1.zip 생성 및 파일 추가
    with zipfile.ZipFile(zip_name, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        # 입력한 파일
        zf.write(input_path, arcname=os.path.basename(input_path))
        # tmp1.txt
        zf.write(tmp_name, arcname=tmp_name)
    print(f"'{zip_name}' 생성 완료.")


# 중간 압축
def create_zip_with_tmpzip(n: int):
    tmp_name = f"tmp{n}.txt"
    new_zip_name = f"zip{n}.zip"
    
    n_before = n-1
    old_zip_name = f"zip{n_before}.zip"

    # 1) tmp1.txt 생성 및 내용 쓰기
    with open(tmp_name, "w", encoding="utf-8") as f:
        f.write(str(n))
        
    # 2) zip1.zip 생성 및 파일 추가
    with zipfile.ZipFile(new_zip_name, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        # 입력한 파일
        zf.write(old_zip_name, arcname=os.path.basename(old_zip_name))
        # tmp1.txt
        zf.write(tmp_name, arcname=tmp_name)
    print(f"'{new_zip_name}' 생성 완료.")


def extract_zip(n: int, dest_dir: str = "."):
    """
    주어진 ZIP 파일을 dest_dir(기본: 현재 작업 디렉터리)에 압축 해제합니다.
    """
    zip_path = f"zip{n}.zip"
    print(zip_path)
    
    # 1) ZIP 파일 존재 여부 확인
    if not os.path.isfile(zip_path):
        print(f"오류: '{zip_path}' 파일을 찾을 수 없습니다.")
        return

    # 2) ZIP 열기 및 추출
    try:
        with zipfile.ZipFile(zip_path, 'r') as zf:
            zf.extractall(dest_dir)
            abs_dest = os.path.abspath(dest_dir)
            print(f"'{zip_path}'의 내용을 '{abs_dest}'에 성공적으로 압축 해제했습니다.")
    except zipfile.BadZipFile:
        print(f"오류: '{zip_path}'은(는) 유효한 ZIP 파일이 아닙니다.")


if __name__ == "__main__":
    # 1. 압축할지, 압축해제 할지 선택
    zip_or_unzip = input("압축 여부를 선택하세요:").strip()

    # 2.A 압축
    zip_count = int( input("압축/압축해제 횟수를 결정해주세요:").strip() )
    if zip_or_unzip == "zip" :
        # 2.A.1 압축할 파일
        file_to_zip = input("압축할 파일 이름을 입력하세요: ").strip()

        # 2.A.2 압축
        for i in range(1,zip_count+1) :
            # 2.A.2.A 첫번째 압축 - 압축파일 생성
            if i==1 : 
                create_zip_with_tmp(file_to_zip, i)

            # 2.A.2.B 이후 압축
            else :
                create_zip_with_tmpzip(i)
    # 2.B 압축해제
    else :
        print("do unzip!")
        for i in range(zip_count, 0, -1) :
            extract_zip(i)
        
    print("done!")
