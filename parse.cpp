#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "./cpp-bencoding/include/BDictionary.h"

void parse(std::string& torrent_file_path){
    std::cout <<"into parse\n";
    std::ifstream fileStream(torrent_file_path, std::ios::binary);
    std::shared_ptr<bencoding::BItem> decoded_torrent_file = bencoding::decode(fileStream);


}

int main(){
    //std::string f1= "sample1.torrent";
    //std::string f1= "./files/ComputerNetworks.torrent";
    std::string f1= "i21e";
    parse(f1);
    return 0;
}