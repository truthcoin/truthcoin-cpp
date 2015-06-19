DIST_SUBDIRS = secp256k1
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)


if EMBEDDED_LEVELDB
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/include
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/helpers/memenv
LIBLEVELDB += $(builddir)/leveldb/libleveldb.a
LIBMEMENV  += $(builddir)/leveldb/libmemenv.a

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
$(LIBLEVELDB): $(LIBMEMENV)

$(LIBLEVELDB) $(LIBMEMENV):
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
          OPT="$(CXXFLAGS) $(CPPFLAGS)"
endif

TRUTHCOIN_CONFIG_INCLUDES=-I$(builddir)/config
TRUTHCOIN_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

TRUTHCOIN_INCLUDES += -I$(srcdir)/secp256k1/include

LIBTRUTHCOIN_SERVER=libtruthcoin_server.a
LIBTRUTHCOIN_WALLET=libtruthcoin_wallet.a
LIBTRUTHCOIN_COMMON=libtruthcoin_common.a
LIBTRUTHCOIN_CLI=libtruthcoin_cli.a
LIBTRUTHCOIN_UTIL=libtruthcoin_util.a
LIBTRUTHCOIN_CRYPTO=crypto/libtruthcoin_crypto.a
LIBTRUTHCOIN_UNIVALUE=univalue/libtruthcoin_univalue.a
LIBTRUTHCOINQT=qt/libtruthcoinqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES = \
  crypto/libtruthcoin_crypto.a \
  libtruthcoin_util.a \
  libtruthcoin_common.a \
  univalue/libtruthcoin_univalue.a \
  libtruthcoin_server.a \
  libtruthcoin_cli.a
if ENABLE_WALLET
TRUTHCOIN_INCLUDES += $(BDB_CPPFLAGS)
EXTRA_LIBRARIES += libtruthcoin_wallet.a
endif

if BUILD_TRUTHCOIN_LIBS
lib_LTLIBRARIES = libtruthcoinconsensus.la
LIBTRUTHCOIN_CONSENSUS=libtruthcoinconsensus.la
else
LIBTRUTHCOIN_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_TRUTHCOIND
  bin_PROGRAMS += truthcoind
endif

if BUILD_TRUTHCOIN_UTILS
  bin_PROGRAMS += truthcoin-cli truthcoin-tx
endif

.PHONY: FORCE
# truthcoin core #
TRUTHCOIN_CORE_H = \
  addrman.h \
  alert.h \
  allocators.h \
  amount.h \
  arith_uint256.h \
  base58.h \
  bloom.h \
  chain.h \
  chainparamsbase.h \
  chainparams.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compressor.h \
  core_io.h \
  crypter.h \
  db.h \
  eccryptoverify.h \
  ecwrapper.h \
  hash.h \
  init.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
  merkleblock.h \
  miner.h \
  mruset.h \
  netbase.h \
  net.h \
  noui.h \
  pow.h \
  primitives/block.h \
  primitives/transaction.h \
  protocol.h \
  pubkey.h \
  random.h \
  rpcclient.h \
  rpcprotocol.h \
  rpcserver.h \
  script/interpreter.h \
  script/script_error.h \
  script/script.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  serialize.h \
  streams.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utilstrencodings.h \
  utiltime.h \
  version.h \
  walletdb.h \
  wallet.h \
  wallet_ismine.h \
  compat/sanity.h

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libtruthcoin_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between truthcoind and truthcoin-qt
libtruthcoin_server_a_CPPFLAGS = $(TRUTHCOIN_INCLUDES) $(MINIUPNPC_CPPFLAGS)
libtruthcoin_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  pow.cpp \
  rest.cpp \
  rpcblockchain.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  script/sigcache.cpp \
  timedata.cpp \
  txdb.cpp \
  txmempool.cpp \
  $(JSON_H) \
  $(TRUTHCOIN_CORE_H)

# wallet: shared between truthcoind and truthcoin-qt, but only linked
# when wallet enabled
libtruthcoin_wallet_a_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
libtruthcoin_wallet_a_SOURCES = \
  db.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  $(TRUTHCOIN_CORE_H)

# crypto primitives library
crypto_libtruthcoin_crypto_a_CPPFLAGS = $(TRUTHCOIN_CONFIG_INCLUDES)
crypto_libtruthcoin_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/common.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/sha1.h \
  crypto/ripemd160.h

# univalue JSON library
univalue_libtruthcoin_univalue_a_SOURCES = \
  univalue/univalue.cpp \
  univalue/univalue_read.cpp \
  univalue/univalue_write.cpp \
  univalue/univalue_escapes.h \
  univalue/univalue.h

# common: shared between truthcoind, and truthcoin-qt and non-server tools
libtruthcoin_common_a_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
libtruthcoin_common_a_SOURCES = \
  allocators.cpp \
  arith_uint256.cpp \
  amount.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  core_read.cpp \
  core_write.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  pubkey.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/sign.cpp \
  script/standard.cpp \
  script/script_error.cpp \
  $(TRUTHCOIN_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libtruthcoin_util_a_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
libtruthcoin_util_a_SOURCES = \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpcprotocol.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(TRUTHCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libtruthcoin_util_a_SOURCES += compat/glibc_compat.cpp
libtruthcoin_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

# cli: shared between truthcoin-cli and truthcoin-qt
libtruthcoin_cli_a_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
libtruthcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(TRUTHCOIN_CORE_H)

nodist_libtruthcoin_util_a_SOURCES = $(srcdir)/obj/build.h
#

# truthcoind binary #
truthcoind_LDADD = \
  $(LIBTRUTHCOIN_SERVER) \
  $(LIBTRUTHCOIN_COMMON) \
  $(LIBTRUTHCOIN_UNIVALUE) \
  $(LIBTRUTHCOIN_UTIL) \
  $(LIBTRUTHCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

if ENABLE_WALLET
truthcoind_LDADD += libtruthcoin_wallet.a
endif
truthcoind_SOURCES = truthcoind.cpp
#

if TARGET_WINDOWS
truthcoind_SOURCES += truthcoind-res.rc
endif

truthcoind_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS)
truthcoind_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
truthcoind_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

# truthcoin-cli binary #
truthcoin_cli_LDADD = \
  $(LIBTRUTHCOIN_CLI) \
  $(LIBTRUTHCOIN_UTIL) \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS)

truthcoin_cli_SOURCES = \
  truthcoin-cli.cpp

truthcoin_cli_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
#

# truthcoin-tx binary #
truthcoin_tx_LDADD = \
  $(LIBTRUTHCOIN_UNIVALUE) \
  $(LIBTRUTHCOIN_COMMON) \
  $(LIBTRUTHCOIN_UTIL) \
  $(LIBTRUTHCOIN_CRYPTO) \
  $(LIBSECP256K1) \
  $(BOOST_LIBS) \
  $(CRYPTO_LIBS)

truthcoin_tx_SOURCES = truthcoin-tx.cpp
truthcoin_tx_CPPFLAGS = $(TRUTHCOIN_INCLUDES)
#
truthcoin_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
truthcoin_cli_SOURCES += truthcoin-cli-res.rc
endif
truthcoin_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if BUILD_TRUTHCOIN_LIBS
include_HEADERS = script/truthcoinconsensus.h
libtruthcoinconsensus_la_SOURCES = \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  primitives/transaction.cpp \
  pubkey.cpp \
  script/truthcoinconsensus.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  uint256.cpp \
  utilstrencodings.cpp

if GLIBC_BACK_COMPAT
  libtruthcoinconsensus_la_SOURCES += compat/glibc_compat.cpp
  libtruthcoinconsensus_la_SOURCES += compat/glibcxx_compat.cpp
endif

libtruthcoinconsensus_la_LDFLAGS = -no-undefined $(RELDFLAGS)
libtruthcoinconsensus_la_LIBADD = $(CRYPTO_LIBS)
libtruthcoinconsensus_la_CPPFLAGS = $(CRYPTO_CFLAGS) -I$(builddir)/obj -DBUILD_TRUTHCOIN_INTERNAL
if USE_LIBSECP256K1
libtruthcoinconsensus_la_LIBADD += secp256k1/libsecp256k1.la
endif
endif

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES)
	$(AM_V_GEN) $(WINDRES) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(CXXFLAGS)  -c -o $@ $<

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(abspath $(<D) $<)

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
