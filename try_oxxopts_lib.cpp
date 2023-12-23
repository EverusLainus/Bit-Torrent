#include <iostream>
#include "/opt/homebrew/include/libtorrent/session.hpp"
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

        libtorrent::torrent_info ti(source_path );
        std::cout << "Torrent name: " << ti.name() << std::endl;
        std::cout << "Piece length: " << ti.piece_length() << " bytes" << std::endl;

        const std::vector<libtorrent::announce_entry>& trackers = ti.trackers();
        std::cout << "Trackers:" << std::endl;
        for (const auto& tracker : trackers) {
            std::cout << "  " << tracker.url << std::endl;
        }

    //} catch (const std::exception& e) {
    //    std::cerr << "Error: " << e.what() << std::endl;
    //    return 1;
    //}

//get peers from the tracker

    libtorrent::session ses;
    
    libtorrent::add_torrent_params params;
    params.ti = std::make_shared <libtorrent::torrent_info>(ti);
    libtorrent::torrent_handle th = ses.add_torrent(params);

    //start the session
    ses.start_upnp();

    while(true){
        ses.post_torrent_updates();

        libtorrent::torrent_status status = th.status();

            // Print information about the number of peers from the tracker
            std::cout << "Number of peers from tracker: " << status.list_peers << std::endl;

        std::vector<libtorrent::peer_info> peers;
        th.get_peer_info(peers);

        for(const auto&peer : peers){
            std::cout << "Peer IP : "<<peer.ip.address().to_string() <<", Port: "<< peer.ip.port() << std::endl;     
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
