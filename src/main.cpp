#include "ufo_torrent.h"

#include <iostream>
//#include <ncurses.h>
#include <curl/curl.h>
#include <string>
#include <vector>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include "../includes/fileUtils.h"
#include "../includes/createHash.h"
#include <boost/locale.hpp>
#include <filesystem>
#include "ncurses_utils.h"

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main()
{
    CURL* curl;
    CURLcode res;
    std::string response;

    // Инициализация libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Установка URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://httpbin.org/get");

        // Установка callback-функции для записи данных
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Выполнение запроса
        res = curl_easy_perform(curl);

        // Проверка на ошибки
        if (res != CURLE_OK) {
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cout << "Ответ сервера:\n" << response << std::endl;
        }

        // Очистка
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}