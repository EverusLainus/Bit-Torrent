#include "ParseTorrentFile.h"


TorrentFileParser::TorrentFileParser(std::string& torrent_file_path){
    
    LOG_F(INFO, "Parsing Torrent file %s...", torrent_file_path.c_str());
    std::ifstream fileStream(torrent_file_path, std::ifstream::binary);
     if (!fileStream.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return ;
    }
    char ch;
    while (fileStream.get(ch)) {
        std::cout << ch;
    }
    std::cout<<" to binary stream " <<std::endl;
    std::shared_ptr<bencoding::BItem> decoded_torrent_file = bencoding::decode(fileStream);
    std::cout<<" decoded_file: " <<decoded_torrent_file<<std::endl;

    std::shared_ptr<bencoding::BDictionary> get_root; 

    try {
    get_root = std::dynamic_pointer_cast<bencoding::BDictionary> (decoded_torrent_file);
    } catch (const bencoding::DecodingError& e) {
    std::cerr << "Decoding error: " << e.what() << std::endl;
    }

    root = get_root;
    LOG_F(INFO, "Parse Torrent file: SUCCESS");
    std::string prettyRepr = bencoding::getPrettyRepr(decoded_torrent_file);
    std::cout << prettyRepr << std::endl;
}

int  main(){
    std::string torrent_file_path = "./files/MoralPsychHandbook.torrent";
    //std::string torrent_file_path = "d4:fouri6ee";
    TorrentFileParser t (torrent_file_path);
    //t()
}
