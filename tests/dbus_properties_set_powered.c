#include <gio/gio.h>
#include <stdio.h>
#include <unistd.h>

#define BLUEZ_BUS_NAME "org.bluez"
#define ADAPTER_OBJECT_PATH "/org/bluez/hci0"
#define ADAPTER_INTERFACE "org.bluez.Adapter1"
#define PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define PROPERTY "Powered"

static void set_power_state(GDBusConnection *connection, gboolean state) {
	GError *error = NULL;
	GVariant *result;
	GVariant *value;

	/* Create the property value variant */
	value = g_variant_new_boolean(state);

	/* Call the Set method on the org.freedesktop.DBus.Properties interface */
	result = g_dbus_connection_call_sync(
			connection,
			BLUEZ_BUS_NAME,                 /* Bus name */
			ADAPTER_OBJECT_PATH,            /* Object path */
			PROPERTIES_INTERFACE,           /* Interface name */
			"Set",                          /* Method name */
			g_variant_new("(ssv)", ADAPTER_INTERFACE, PROPERTY, value), /* Parameters */
			NULL,                           /* Expected return type */
			G_DBUS_CALL_FLAGS_NONE,         /* Flags */
			-1,                             /* Timeout (use default) */
			NULL,                           /* Cancellable */
			&error                          /* Error */
			);

	if (error != NULL) {
		fprintf(stderr, "Error setting power state: %s\n", error->message);
		g_error_free(error);
	} else {
		printf("Set power state to %s\n", state ? "on" : "off");
	}

	if (result != NULL) {
		g_variant_unref(result);
	}
}

int main(void)
{
	GDBusConnection *connection;
	GError *error = NULL;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error != NULL) {
		fprintf(stderr, "Error connecting to system bus: %s\n", error->message);
		g_error_free(error);
		return 1;
	}

	/* Turn off the adapter */
	set_power_state(connection, FALSE);

	sleep(3);

	/* Turn on the adapter */
	set_power_state(connection, TRUE);

	/* Clean up and exit */
	g_object_unref(connection);
	return 0;
}
