# Wolf QUIC

Wolf provides quic protocol as a submodule under stream module. Current implementation is mostly a C++ wrapper over msquic which is written in C, along with a few helpers to simplify and help with common usages such as client/server.

There are one to one C++ wrapper types for msquic types. including enums and flags, core data types to hold information, and handle types. public methods and member variables are also kind of one to one with similar name, sometimes with a little modification to ease the use.

Handle instances which are async and have callbacks (connection and stream) have internal refcounting, and when running an instance of them will be managed internally and won't be destroyed/closed until stopped or are shut down. this allows, for example, one to start running an instance of connection in a listener callback and not to worry of managing its lifetime. and if an instance wasn't started, once its out of scope, it will automatically be closed.

Due to the way msquic was designed, the parentship of instances like stream by connection and connection/listener by registration must be handled by user of this library. for example: when a connection is closed, the stream will automatically shut down and user must ensure that stream instances held by them will no longer be accessed or avilable.

So make sure dependent handles **DO NOT** outlive their dependencies. hopefully in the future this will be managed internally.

To understand these dependencies please refer to `API` overview documenetation of msquic: https://github.com/microsoft/msquic/blob/main/docs/API.md

## Base Data Types

Msquic enums/flags have an equivalent enum classes to be used. for bit flags, `w_flags` is used to handle bitwise operations.

The header `stream/quic/datatypes/common_flags.hpp` defines common enum flags used throughout the project.

For alpn name, since it is not forced to be a string, there is `w_alpn_view` in `stream/quic/datatypes/w_alpn.hpp` which represents an alpn name and is simply an alias to `std::span<uint8_t>`. it comes with `as_alpn_view` helper method to create view from string views and c strings.

Here is a list of C++ wrapper types and their corresponding msquic types:
 - `w_address` represents `QUIC_ADDRESS`.

 - `w_status` and `w_status_code` represent `QUIC_STATUS`. the first is for holding a status code and checking whether it succeeded or failed, and the latter is an enum representing different status codes defined by msquic.

 - `w_settings` represents `QUIC_SETTINGS`, having each field as an `std::optional` so it can be easily aggregate initialized with specific field. e.g. `w_settings { .idle_timeout_ms = 2000 }`.

 - `w_credential_config` represents `QUIC_CREDENTIAL_CONFIG`, and for convenience there are `w_certificate_<something>` alongside it to represent `QUIC_CERTIFICATE_<SOMETHING>` and be used with ease to construct a credential config with any certificate without worrying about value semantics.

 - `w_registration_config` represents `QUIC_REGISTRATION_CONFIG`, and its fields are accessible through set/get methods.

 - `w_new_connection_info` represents `QUIC_NEW_CONNECTION_INFO`, and its fields are readonly, accessible through const methods with same name.

## Events

Just as msquic uses callbacks with event as a parameter, those event types are wrapped and represented by classes.

Event types are similar to `std::variant`, and to access the underlying event value, it must be visited with an overload set. wolf library provides `system/w_overloaded` to overload lambdas to ease this.

All event types also have a `name` method to get the underlying type's name string.

There are no fields, but wrapper classes for each specific event, named as `w_<handle-name>_event_<event-name>`, e.g. `w_listener_event_new_connection`. these event classes hold the underlying data, and provide getter and helper methods to interact with them.

Events can not be created, copied, or moved. their lifetime is solely in hands of wolf library that passes them to callback/visitor.

 - `w_listener_event` represents `QUIC_LISTENER_EVENT`. in `w_listener_event_new_connection` there are accept/reject methods which shouldn't be called more than once or over each other, and if neither is called then the connection will be rejected.

 - `w_conection_event` represents `QUIC_CONNEcTION_EVENT`, and just like `w_listener_event_new_connection` there is `w_connection_event_peer_stream_started` that has accept/reject methods and by default rejects if neither is called.

 - `w_stream_event` represents `QUIC_STREAM_EVENT`.

## Handles

The handles in msquic all have the type of `HQUIC`. but here are the common concepts and kinds of handles provided by msquic which have their own api, and wolf has wrapper for them:

 - Registration: `w_registration` which on destruction waits on its children to be closed.

 - Configuration: `w_configuration`.

 - Listener: `w_listener` which starts and works async with callback, but it will shutdown when its last instance goes out of scope.

 - Connection: `w_connection` which starts and works async with callback, and will be kept alive/running even if there are no instances of it kept by user. thus user can easily accept/open a new connection and not have to worry about its lifetime.

 - Stream: `w_stream` which starts and works async with callbacks, and will be kept alive/running even if there are no instances of it kept by user. thus user can easily accept/open a new stream and not have to worry about its lifetime.

Handles can not be constructed and are created by (commonly named `open`) factory static methods, using boost::leaf for error report and handling.

Callbacks passed to create/set_callback of handle can be any callable and its storage will be internally managed.

Wolf doesn't provide the `context` pointer concept of msquic to user of this library, so instead use lambda captures or other C++-based alternatives.

## For Developer/Contributor (Internals)

Msquic's api table is created only once and is provided by singleton-style factory method `internal::w_msquic_api::api`.

As a C wrapper library there are many scenarios which one C function/type requires another C instance, and for this reason `internal::w_raw_access` pattern is used. classes which wrap a raw/C type declare that as `freind` class and provide desired private api so in any other part in the library if raw access is required, it can be gained via `internal::w_raw_access` can be used and these two types/methods need not to know of each other (necessary for generics). saving us tons of forward declarations and maintenance bookkeepings.

Handles are kept lightweight, only have the `HQUIC` pointer as data member, and all of their required shared data is allocated on heap (the type `context_bundle`) and saved in the handle's context. so the context pointer must never change/written and should be always read for concurrency safety.
