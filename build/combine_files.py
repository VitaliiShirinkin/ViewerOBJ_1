# combine_files.py
import os

def combine_files(source_dir, output_file):
    # Открываем выходной файл для записи
    with open(output_file, 'w', encoding='utf-8') as outfile:
        # Проходим по всем файлам в исходной директории
        for root, _, files in os.walk(source_dir):
            for file in files:
                if file.endswith(('.cpp', '.h')):
                    file_path = os.path.join(root, file)
                    # Записываем имя файла как комментарий
                    outfile.write(f"// File: {file}\n")
                    # Читаем и записываем содержимое файла
                    with open(file_path, 'r', encoding='utf-8') as infile:
                        outfile.write(infile.read())
                    outfile.write("\n\n")  # Добавляем пустые строки между файлами

if __name__ == "__main__":
    # Путь к исходной директории (папка проекта)
    source_dir = os.path.join(os.path.dirname(__file__), "..")
    # Путь к выходному файлу (в папке build)
    output_file = os.path.join(os.path.dirname(__file__), "combined_code.cpp")
    # Выполняем объединение
    combine_files(source_dir, output_file)
    print(f"Файлы объединены в {output_file}")
