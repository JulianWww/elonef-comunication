#include <elonef-communication/handlers/data/client_connection_data.hpp>

Elonef::ClientConnectionData::ClientConnectionData() :
    auth_data(new DataWaiter<ClientAuthData>())
{}