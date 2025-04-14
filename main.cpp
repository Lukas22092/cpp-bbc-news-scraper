#include <iostream>
#include <curl/curl.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>

// Account for edge cases where keywords like "News" or "Sport" would otherwise get displayed
bool check_title_validity(std::string& title){
    int count =0;
    std::stringstream ss(title);
    std::string word;
    while(ss>>word){
        count++;
    }
    return count<=3;
}
// Write received data chunks into the response string
size_t write_chunk(void *data, size_t size, size_t nmemb, void *user_data) {
    size_t real_size = size * nmemb;
    std::string *response = (std::string*)user_data;
    response->append((char*)data, real_size);
    return real_size;
}
// Send an HTTP GET request to the BBC News page
bool send_request(std::string &response_str) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Curl initialization failed!" << std::endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbc.com/news");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_str);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "Error requesting the webpage: " << curl_easy_strerror(result) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_cleanup(curl);
    return true;
};
// Save the website`s HTML structure to a file
void write_to_txt(std::string &response){

    std::ofstream myfile;
    myfile.open ("BBC_website_structure.txt");
    myfile << response;
    myfile.close();
};

int main() {
    std::string response;

    if(!send_request(response)){
        std::cout<<"Response failed!"<<std::endl;}
        else{std::cout<<"Response received!"<<std::endl<<std::endl;};
    write_to_txt(response);
    std::regex titlePattern(R"delim("title"\s*:\s*"([^"]+)")delim");

    std::sregex_iterator end;

    //itterate trough the pattern 
    auto itTitle = std::sregex_iterator(response.begin(), response.end(), titlePattern);

    while (itTitle != end) {
        std::string title = (*itTitle)[1].str();
        if(!check_title_validity(title)){

        std::cout << "Title: " << title << std::endl;
        ++itTitle;}
    }
    return 0;
}
