#include <iostream>
#include "/opt/homebrew/include/libtorrent/torrent_info.hpp"
#include "/opt/homebrew/include/libtorrent/announce_entry.hpp"
#include "cxxopts.hpp"

int main(int argc, char* argv[]) {
    cxxopts::Options options("MyProgram", " - example command line options");
    std::string source_path;
    std::string destination_path;

    options.add_options()
        ("h,help", "Print help. " )
        ("s,source", "source file", cxxopts::value<std::string>(source_path))
        ("d,destination", "destination file", cxxopts::value<std::string>(destination_path));

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("source")) {
        std::cout<<"source path: " <<source_path << std::endl;
    }

    if (result.count("destination")) {
        std::cout<<"destination path: " <<destination_path << std::endl;
    }

        try {
        libtorrent::torrent_info ti(source_path );
        std::cout << "Torrent name: " << ti.name() << std::endl;
        std::cout << "Piece length: " << ti.piece_length() << " bytes" << std::endl;

        const std::vector<libtorrent::announce_entry>& trackers = ti.trackers();
        std::cout << "Trackers:" << std::endl;
        for (const auto& tracker : trackers) {
            std::cout << "  " << tracker.url << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
