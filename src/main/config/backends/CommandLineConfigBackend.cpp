//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#include "CommandLineConfigBackend.hpp"

#include <boost/asio/version.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace ls {

	CommandLineConfigBackend::CommandLineConfigBackend(ConfigStore& store)
		: configStore(store) {
	}

	void CommandLineConfigBackend::Process(int argc, char** argv) {
		po::options_description desc("SSX 3 Lobby Server - Options");
		po::variables_map vm;
		try {
			// clang-format off
			desc.add_options()
				("listen,l", po::value<std::string>(), "The address to listen on. Defaults to all interfaces.")
				("version,V", "Display server versions")
				("verbose,v", "Enable verbose log messages (for debugging purposes)")
				("help,h", "Show this help");

			// clang-format on

			po::store(po::parse_command_line(argc, argv, desc), vm);

			if(vm.count("help")) {
				std::cout << desc << "\n";
				std::exit(0);
			}

			if(vm.count("version")) {
				std::cout << "SSX 3 Lobby Server\n\n"
						  << "Compiled with:\n"
						  // library versions
						  << "\t\b\b\b\b- Boost " << BOOST_VERSION / 100000 << '.' << BOOST_VERSION / 100 % 1000 << '\n'
						  << "\t\b\b\b\b- Boost.Asio " << BOOST_ASIO_VERSION / 100000 << '.' << BOOST_ASIO_VERSION / 100 % 1000 << '.' << BOOST_ASIO_VERSION % 100 << '\n';
				std::exit(0);
			}

			if(vm.count("listen"))
				configStore.SetValue("listen_address", vm["listen"].as<std::string>());

			po::notify(vm);
		} catch(boost::program_options::required_option::error& e) {
			std::cerr << "Error: " << e.what() << '\n';
			std::exit(1);
		} catch(std::exception& e) {
			std::cerr << e.what() << "\n";
			std::exit(1);
		} catch(...) {
			std::cerr << "An unknown error has occurred\n";
			std::exit(1);
		}
	}

} // namespace ls