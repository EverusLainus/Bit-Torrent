#include <bencoding.h>
#include <loguru/loguru.hpp>
/*
*parse torrent file
takes in the path of the torrent file

*/

class TorrentFileParser{
    explicit TorrentFileParser(std::string& torrent_file_path);
    std::shared_ptr<bencoding::BDictionary> root;

};