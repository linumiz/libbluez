#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>

#define BLUEZ_BUS_NAME			"org.bluez"
#define ADAPTER_INTERFACE		"org.bluez.Adapter1"
#define DBUS_PROPERTIES_INTERFACE	"org.freedesktop.DBus.Properties"
#define DBUS_METHOD_GET			"Get"
#define ADAPTER_UUIDS_PROPERTY		"UUIDs"
#define DEFAULT_ADAPTER_PATH		"/org/bluez/hci0"

static void get_uuids_from_adapter(const char *adapter_object_path)
{
	GDBusConnection *connection;
	GError *error = NULL;
	GVariant *reply;
	GVariant *uuids_variant;
	const gchar **uuids;
	int i;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error) {
		g_printerr("Error connecting to system bus: %s\n", error->message);
		g_error_free(error);
		return;
	}

	/* Call the Get method on the adapter's 'org.freedesktop.DBus.Properties' interface */
	reply = g_dbus_connection_call_sync(
			connection,
			BLUEZ_BUS_NAME,                     /* Bus name */
			adapter_object_path,                /* Object path */
			DBUS_PROPERTIES_INTERFACE,          /* Interface name */
			DBUS_METHOD_GET,                    /* Method name */
			g_variant_new("(ss)", ADAPTER_INTERFACE, ADAPTER_UUIDS_PROPERTY), /* Parameters */
			G_VARIANT_TYPE("(v)"),              /* Expected reply type */
			G_DBUS_CALL_FLAGS_NONE,             /* Call flags */
			-1,                                 /* Timeout (default) */
			NULL,                               /* Cancellable */
			&error                              /* Error */
			);
	if (error) {
		g_printerr("Error calling Get method: %s\n", error->message);
		g_error_free(error);
		g_object_unref(connection);
		return;
	}

	/* Parse the reply to extract the UUIDs */
	g_variant_get(reply, "(v)", &uuids_variant);
	g_variant_get(uuids_variant, "^a&s", &uuids);

	/* Print the UUIDs */
	for (i = 0; uuids[i] != NULL; i++)
		g_print("UUID[%d]: %s\n", i, uuids[i]);

	g_variant_unref(uuids_variant);
	g_variant_unref(reply);
	g_object_unref(connection);
}

int main(void)
{
	const char *adapter_path = DEFAULT_ADAPTER_PATH;

	/* Call the function to get UUIDs */
	get_uuids_from_adapter(adapter_path);

	return 0;
}
