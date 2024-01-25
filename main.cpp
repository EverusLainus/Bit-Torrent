#include <iostream>
#include <fstream>
#include "cxxopts.hpp"
#include "ParseTorrentFile.h"
#include "RequestTracker.hpp"
#include "Connect.h"
#include "cpp-bencoding/include/bencoding.h"
#include "loguru.hpp"

int main(int argc, char* argv[]) {
    cxxopts::Options options("MyProgram", " - example command line options");
    std::string source_path;
    std::string destination_path;
    std::string log_file;

    options.add_options()
        ("h,help", "Print help. " )
        ("s,source", "source file", cxxopts::value<std::string>(source_path))
        ("d,destination", "destination file", cxxopts::value<std::string>(destination_path))
        ("l,log", "log file", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("source")) {
        std::cout<<"source path: " <<source_path << std::endl;

    }
    else{
        return -1;
    }

    if (result.count("destination")) {
        std::cout<<"destination path: " <<destination_path << std::endl;
    }

    if (result.count("log"))
    {
        log_file = result["log"].as<std::string>();
        loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
        loguru::g_flush_interval_ms = 100;
        loguru::add_file(log_file.c_str(), loguru::Truncate, loguru::Verbosity_MAX);
    }

        std::string torrent_file_path = result["source"].as<std::string>();
    destination_path = result["destination"].as<std::string>();
    
    //TorrentFileParser torrentFileParser(torrent_file_path);

LOG_F(INFO, "Downloading thread started...");
//get peers from the tracker
TorrentFileParser torrentFileParser(torrent_file_path);
    TrackerRequest trackerRequest(torrentFileParser);
    std::string fileName = torrentFileParser.getFileName();
    trackerRequest.downloadPath = destination_path + fileName;
    trackerRequest.get_peers(torrent_file_path);
    std::cout << " main: out of get peers"<<std::endl;
    trackerRequest.decodeResponse(trackerRequest.TrackerResponse);

    trackerRequest.ConnectPeer();

 // Close the file


    return 0;
}
