#include <gio/gio.h>
#include <stdio.h>

int main(void)
{
	GDBusConnection *connection;
	GError *error = NULL;
	GVariant *result;
	GVariant *value;
	gboolean powered;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error != NULL) {
		g_printerr("Error connecting to system bus: %s\n", error->message);
		g_error_free(error);
		return 1;
	}

	result = g_dbus_connection_call_sync(
			connection,
			"org.bluez",                   /* Bus name of the BlueZ service */
			"/org/bluez/hci0",             /* Object path of the adapter */
			"org.freedesktop.DBus.Properties", /* Interface name */
			"Get",                         /* Method name */
			g_variant_new("(ss)", "org.bluez.Adapter1", "Powered"), /* Parameters */
			G_VARIANT_TYPE("(v)"),         /* Expected return type */
			G_DBUS_CALL_FLAGS_NONE,
			-1,                            /* Default timeout */
			NULL,                          /* GCancellable */
			&error
			);
	if (error != NULL) {
		g_printerr("Error calling Get method: %s\n", error->message);
		g_error_free(error);
		g_object_unref(connection);
		return 1;
	}

	g_variant_get(result, "(v)", &value);
	g_variant_get(value, "b", &powered);

	g_print("Powered: %s\n", powered ? "true" : "false");

	g_variant_unref(value);
	g_variant_unref(result);
	g_object_unref(connection);

	return 0;
}
