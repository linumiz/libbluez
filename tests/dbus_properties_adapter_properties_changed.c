#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLUEZ_BUS_NAME			"org.bluez"
#define ADAPTER_INTERFACE		"org.bluez.Adapter1"
#define ADAPTER_OBJECT_PATH		"/org/bluez/hci0"
#define PROPERTIES_CHANGED_SIGNAL	"PropertiesChanged"
#define ALIAS_PROPERTY			"Alias"

static void on_properties_changed(GDBusConnection *connection,
                      const gchar *sender_name,
                      const gchar *object_path,
                      const gchar *interface_name,
                      const gchar *signal_name,
                      GVariant *parameters,
                      gpointer user_data)
{
	(void)connection;
	(void)sender_name;
	(void)object_path;
	(void)interface_name;
	(void)signal_name;
	(void)user_data;

	GVariantIter *properties_iter;
	const gchar *key;
	GVariant *value;

	g_variant_get(parameters, "(sa{sv}as)", NULL, &properties_iter, NULL);

	while (g_variant_iter_next(properties_iter, "{sv}", &key, &value)) {
		if (g_strcmp0(key, ALIAS_PROPERTY) == 0) {
			const gchar *alias;
			alias = g_variant_get_string(value, NULL);
			printf("Property Changed: %s -> %s\n", key, alias);
		}
		g_variant_unref(value);
	}

	g_variant_iter_free(properties_iter);
}

static void set_alias_property(GDBusConnection *connection, const gchar *alias)
{
	GError *error = NULL;
	GVariant *result;

	result = g_dbus_connection_call_sync(connection,
			BLUEZ_BUS_NAME,
			ADAPTER_OBJECT_PATH,
			"org.freedesktop.DBus.Properties",
			"Set",
			g_variant_new("(ssv)",
				ADAPTER_INTERFACE,
				ALIAS_PROPERTY,
				g_variant_new_string(alias)),
			NULL,
			G_DBUS_CALL_FLAGS_NONE,
			-1,
			NULL,
			&error);

	if (error != NULL) {
		fprintf(stderr, "Error setting alias: %s\n", error->message);
		g_clear_error(&error);
	} else
		printf("Alias set to: %s\n", alias);

	g_variant_unref(result);
}

int main(int argc, char **argv)
{
	GDBusConnection *connection;
	GError *error = NULL;
	guint subscription_id;

	if (argc != 2) {
		fprintf(stderr, "%s <alias name>", argv[0]);
		return 1;
	}

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error != NULL) {
		fprintf(stderr, "Error connecting to system bus: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	subscription_id = g_dbus_connection_signal_subscribe(connection,
			BLUEZ_BUS_NAME,
			"org.freedesktop.DBus.Properties",
			PROPERTIES_CHANGED_SIGNAL,
			ADAPTER_OBJECT_PATH,
			ADAPTER_INTERFACE,
			G_DBUS_SIGNAL_FLAGS_NONE,
			on_properties_changed,
			NULL,
			NULL);

	/* Set the alias property */
	set_alias_property(connection, argv[1]);

	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	g_dbus_connection_signal_unsubscribe(connection, subscription_id);
	g_main_loop_unref(loop);
	g_object_unref(connection);

	return 1;
}
