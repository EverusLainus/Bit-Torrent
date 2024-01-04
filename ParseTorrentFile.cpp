#include "ParseTorrentFile.h"
#include "sha1.h"
#include "./cpp-bencoding/include/bencoding.h"



TorrentFileParser::TorrentFileParser(const std::string& torrent_file_path){
    std::cout << " constructed torrent file parser"<<std::endl;

    try{
    
    //LOG_F(INFO, "Parsing Torrent file %s...", torrent_file_path.c_str());
    
    std::ifstream fileStream(torrent_file_path, std::ios::binary);
     if (!fileStream.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return ;
    }
    char ch;
    std::string str;

    while (fileStream.get(ch)) {
        //std::cout << ch;
        str.push_back(ch);
    }
    std::cout<< "string is "<<str<<std::endl;
    

    std::cout<<" to binary stream " <<std::endl;
    
    std::shared_ptr<bencoding::BItem> decoded_torrent_file = bencoding::decode(str);
    std::cout<<" decoded_file: " << decoded_torrent_file <<std::endl;

    std::shared_ptr<bencoding::BDictionary> get_root; 

    try {
    get_root = std::dynamic_pointer_cast<bencoding::BDictionary> (decoded_torrent_file);
    } catch (const bencoding::DecodingError& e) {
    std::cerr << "Decoding error: " << e.what() << std::endl;
    }

    root = get_root;
    //LOG_F(INFO, "Parse Torrent file: SUCCESS");
    std::string prettyRepr = bencoding::getPrettyRepr(decoded_torrent_file);
    std::cout << prettyRepr << std::endl;
    } catch(const bencoding::DecodingError &e){
        std::cout <<"error: "<< e.what() <<std::endl;
    }
    std::cout << "end of constructed torrent file parser"<<std::endl;
}
/*
int  main(){
    std::string torrent_file_path = "./files/MoralPsychHandbook.torrent";
    //std::string torrent_file_path = "d4:fouri6ee";
    TorrentFileParser t(torrent_file_path);
    //t()
}
*/
/**
 * gets the value of the given key
*/

std::shared_ptr<bencoding::BItem> TorrentFileParser::get(std::string key){
    std::shared_ptr <bencoding::BItem> value = root->getValue(key);
    return value;
}

std::string TorrentFileParser::getAnnounce(){
    std::shared_ptr <bencoding::BItem>anounceUrl = TorrentFileParser::get("announce");
    std::string return_string = std::dynamic_pointer_cast<bencoding::BString> (anounceUrl)->value();
    return return_string;
}

/*
std::string calculate_sha1_hash(const std::string& input_string) {
    SHA_CTX sha1_context;
    SHA1_Init(&sha1_context);
    SHA1_Update(&sha1_context, input_string.c_str(), input_string.length());

    unsigned char sha1_hash[SHA_DIGEST_LENGTH];
    SHA1_Final(sha1_hash, &sha1_context);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)sha1_hash[i];
    }

    return ss.str();
}
*/

std::string TorrentFileParser::getInfoHash(){
    std::cout << "getinforhash : in "<<std::endl;
    std::shared_ptr <bencoding::BItem>info = TorrentFileParser::get("info");
    std::string encoded_info = bencoding::encode(info);
    std::cout <<"encoded info is " <<encoded_info<<std::endl;

    SHA1 sha1;
    sha1.update(encoded_info);
    std::string final = sha1.final();
    std::cout <<"sha1 encoded info is " <<final<<std::endl;
    /*
    std::string sha_encoded_info = sha1(encoded_info);
    std::cout <<"sha1 encoded info is " <<sha_encoded_info<<std::endl;
    std::string truncatedHash = sha_encoded_info.substr(0, 40);
    std::cout <<"truncatedHash is " <<truncatedHash<<std::endl;
    //std::string return_string = std::dynamic_pointer_cast<bencoding::BString> (info_hash)->value();
    return truncatedHash;
    */
   return final;
}

int TorrentFileParser::getFileSize(){
    std::shared_ptr <bencoding::BItem> fileSize = TorrentFileParser::get("length");
    int return_int = std::dynamic_pointer_cast<bencoding::BInteger> (fileSize)->value();
    return return_int;
}

