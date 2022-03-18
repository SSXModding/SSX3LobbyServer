//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

// Since we're using (BOOST_)ASIO_SEPARATE_COMPILATION, we need
// to include the <(boost/)asio/impl/src.hpp> header in some TU.
// We use this one to explicitly do so.

#include <boost/asio/impl/src.hpp>