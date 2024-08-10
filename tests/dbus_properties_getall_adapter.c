#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>

#define BLUEZ_BUS_NAME			"org.bluez"
#define ADAPTER_INTERFACE		"org.bluez.Adapter1"
#define DBUS_PROPERTIES_INTERFACE	"org.freedesktop.DBus.Properties"
#define DBUS_METHOD_GET_ALL		"GetAll"
#define DEFAULT_ADAPTER_PATH		"/org/bluez/hci0"

static GDBusConnection* connect_to_system_bus(GError **error)
{
	GDBusConnection *connection;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
	return connection;
}

static GVariant* call_get_all_properties(GDBusConnection *connection, const char *adapter_object_path, GError **error)
{
	GVariant *reply;

	reply = g_dbus_connection_call_sync(
			connection,
			BLUEZ_BUS_NAME,                     /* Bus name */
			adapter_object_path,                /* Object path */
			DBUS_PROPERTIES_INTERFACE,          /* Interface name */
			DBUS_METHOD_GET_ALL,                /* Method name */
			g_variant_new("(s)", ADAPTER_INTERFACE), /* Parameters */
			G_VARIANT_TYPE("(a{sv})"),          /* Expected reply type */
			G_DBUS_CALL_FLAGS_NONE,             /* Call flags */
			-1,                                 /* Timeout (default) */
			NULL,                               /* Cancellable */
			error                               /* Error */
			);

	return reply;
}

static void print_adapter_properties(GVariant *properties)
{
	GVariantIter *iter;
	GVariant *value;
	gchar *key;

	g_variant_get(properties, "(a{sv})", &iter);
	while (g_variant_iter_loop(iter, "{&sv}", &key, &value))
	{
		gchar *value_str = g_variant_print(value, TRUE);
		g_print("Property: %s = %s\n", key, value_str);
		g_free(value_str);
	}
	g_variant_iter_free(iter);
}

static void get_all_adapter_properties(const char *adapter_object_path)
{
	GDBusConnection *connection;
	GError *error = NULL;
	GVariant *reply;

	connection = connect_to_system_bus(&error);
	if (error)
	{
		g_printerr("Error connecting to system bus: %s\n", error->message);
		g_error_free(error);
		return;
	}

	/* Call the GetAll method */
	reply = call_get_all_properties(connection, adapter_object_path, &error);
	if (error)
	{
		g_printerr("Error calling GetAll method: %s\n", error->message);
		g_error_free(error);
		g_object_unref(connection);
		return;
	}

	print_adapter_properties(reply);

	g_variant_unref(reply);
	g_object_unref(connection);
}

int main(void)
{
	const char *adapter_path = DEFAULT_ADAPTER_PATH;

	get_all_adapter_properties(adapter_path);

	return 0;
}
