# librist. Copyright (c) 2020 SipRadius LLC. All right reserved.
# SPDX-License-Identifier: BSD-2-Clause

version_h_data = configuration_data()

version_h_data.set('LIBRIST_API_VERSION_MAJOR', librist_api_version_major)
version_h_data.set('LIBRIST_API_VERSION_MINOR', librist_api_version_minor)
version_h_data.set('LIBRIST_API_VERSION_PATCH', librist_api_version_patch)

librist_api_version = '0x@0@'.format(librist_api_version_major * 10000 + librist_api_version_minor  * 100 + librist_api_version_patch)

version_h_data.set('LIBRIST_API_VERSION', librist_api_version)

version_h_target = configure_file(input: 'version.h.in',
								  output: 'version.h',
								  configuration: version_h_data)

config_h_data = configuration_data()
config_h_data.set10('HAVE_MBEDTLS', mbedcrypto_lib_found)
config_h_target = configure_file(input: 'config.h.in',
								 output: 'librist_config.h',
								 configuration: config_h_data)

if should_install
	install_headers('common.h',
					'headers.h',
					'librist_srp.h',
					'librist.h',
					'logging.h',
					'oob.h',
					'opt.h',
					'peer.h',
					'receiver.h',
					'sender.h',
					'stats.h',
					'udpsocket.h',
					'urlparam.h',
					version_h_target,
					config_h_target,
					subdir: 'librist')
endif
