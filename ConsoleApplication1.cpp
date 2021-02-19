#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

long long sizeoffile(fs::path pwd, std::ifstream& in) {     // функция, выводящая размер файла по 8 байт
    long long size;
    in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {                                                                       // через try/catch обрабатывает 
        in.open(pwd, std::ios::binary | std::ios::ate);              // исключения
        size = in.tellg();              // а сам размер ищет через метод tellg
        in.close();
        return size;
    }
    catch (std::exception& wrong) {
        std::cout << "error: " << wrong.what();
        return 15;
    }
}

int filetype(fs::path pwd) {         //функция, которая выводит номер расширения файла в нашей таблице
    
    fs::path typefs = pwd.extension();
    std::string typestr{ pwd.string() };
    std::string extencion[16] = { "doc", "txt", "mp3", "pdf", "png", "jpg", "cpp", "docx", "mp4", "jpeg","gif", "xls","ppt", "wav","exe", "rtf", };
    for (int i = 0; i < 16; i++) {
        if (extencion[i] == typestr)
            return i;
    }
    return -1;
}

void extensionstobyte(std::vector <char> header, std::vector <fs::path> pwd, short counterF) {  // упаковывает расширения в 1 байт
    for (int i = 0; i < counterF; i++) {
        if (i % 2 == 0)
            header[i / 2] = filetype(pwd[i]) << 4;        //используя бинарный сдвиг на 4  бита влево, т.к. расширение
        else                                           // занимает 4 бита
            header[i / 2] += filetype(pwd[i]);
    }
}

int main() {
    std::ifstream in;          //создаём переменные типа ifstream и ofstream
    std::ofstream out;

    std::cout << "Enter count of files: ";
    short counterF = 0;         // создаём переменную, хранящую количество файлов размером в 2 байта
    std::cin >> counterF;

    std::vector <fs::path> pwd(counterF);       // массив путей до файлов
    std::vector <long long> filesize(counterF);    // размеров
    std::vector <std::string> filetype(counterF); // типов 
    std::vector <char> header(16);        // заголовок файла


    for (int i = 0; i < counterF; i++) {
        std::cout << "Enter path to input file: ";
        std::cin >> pwd[i];
    }
    for (int i = 0; i < counterF; i++)                  //  вводим пути и определяем размеры файлов
        filesize[i] = sizeoffile(pwd[i], in);      //

   // std::cout << "Enter path to outfile: ";
   // std:: string path;
   // std::cin >> path;
   // path = path + "outfile.bin" ;
    
    fs::path path = ".outfile.bin";
    

    out.open(path, std::ios::binary);   //открываем поток на запись
    out.write(reinterpret_cast<char*>(&counterF), 2);            //записываем кол-во файлов, преобразованные в char в битовом представлении
    for (int i = 0; i < counterF; i++) {
        out.write(reinterpret_cast<char*>(&filesize[i]), 8);     //запись размеров файлов, преобразованные в char в в битовом представлении
    }
    extensionstobyte(header, pwd, counterF);        // упаковываем расширения в заголовок
    for (int i = 0; i < counterF; i++) {
        out.write(reinterpret_cast<char*>(&header[i]), 1);
    }       // записываем заголовок


        for (int i = 0; i < counterF; i++) {
            std::ifstream in(pwd[i], std::ifstream::binary);  //открываем поток чтения
            while (!in.eof())
            {
                char buffer[1024]; // создаём буфер
                in.read(buffer, 1024);                //  читаем файлы 
                int readed = in.gcount();            //количество символов, прочитанных при вводе
                out.write(buffer, readed);           // записываем содержимое файлов в общий файл
                
            }
            in.close();
        }
        
        delete[] header;
        return 0;
    }
