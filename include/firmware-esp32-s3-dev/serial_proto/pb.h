/*
 * Shim for generated headers (`#include <pb.h>`).
 *
 * Real nanopb lives in vendor/nanopb/. CMake and clangd put
 * serial_proto/ on -I before (or without) vendor/nanopb; this
 * file makes <pb.h> resolve either way.
 */
#include "../vendor/nanopb/pb.h"
