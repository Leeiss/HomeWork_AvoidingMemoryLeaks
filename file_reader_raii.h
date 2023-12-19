#include <cstdio>
#include <exception>
#include <string>
#include <fstream>
 
class CannotOpenFileException : public std::runtime_error {
public:
    CannotOpenFileException() : std::runtime_error("Cannot open file") {}
};

class FileRaiiWrapper {
private:
    std::FILE* f; // тот самый файловый дескриптор, который мы оборачиваем

public:
// если файл не открылся, например его нет на диске, то нужно выбросить ошибку CannotOpenFileException
    FileRaiiWrapper(const std::string& name) {
        // реализации открытия файла и выброса ошибки тут
        f = std::fopen(name.c_str(), "r");//открываем файл для чтения
        if (f == nullptr) {
            throw CannotOpenFileException();

        }
    }
    FileRaiiWrapper(const FileRaiiWrapper&) = delete; //удаляем конструкор копирования
    FileRaiiWrapper& operator = (const FileRaiiWrapper&) = delete; // удаляем конструктор присваивания

    // File&& — ссылка на временный объект
    // забираем владение дескриптором у временного объекта other
    // Конструктор перемещения
    FileRaiiWrapper(FileRaiiWrapper&& other) noexcept { 
        // реализация перемещения тут
        f = nullptr;
        std::swap(f, other.f);
    }

    // Оператор присваивания с семантикой перемещения
    FileRaiiWrapper& operator=(FileRaiiWrapper&& other) noexcept {
        // реализация перемещения тут
        if (this != &other) {
            if (f) std::fclose(f);//если f - не нулевой указатель, то есть файл уже открыт, для очистки
            f = nullptr;
            std::swap(f, other.f);
        }
        return *this;
    }

    // Закрываем файл в деструкторе, если он валидный
    ~FileRaiiWrapper() noexcept {
        if (f) std::fclose(f);
    }
    // функция для чтения строки длинной bytes_count из файла
    std::string ReadBytes(const size_t bytes_count) const {
        if (!f) {
            return ""; 
        } 
        std::unique_ptr<char[]> buf(new char[bytes_count]{}); //класс для динамическо выделенной памяти 
        size_t bytesRead = std::fread(buf.get(), 1, bytes_count, f);//считываем в буфер
        std::fclose(f);
        return std::string(buf.get(), bytesRead);
    }
};