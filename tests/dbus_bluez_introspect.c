#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define BLUEZ_BUS_NAME		"org.bluez"
#define BLUEZ_OBJECT_PATH	"/org/bluez/hci0"
#define INTROSPECT_METHOD	"Introspect"

int main(void)
{
	GDBusConnection *connection;
	GError *error = NULL;
	GVariant *result;
	const char *xml_data;
	xmlChar *xmlbuff;
	xmlDocPtr doc;
	int buffersize;
	int ret = 0;

	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error != NULL) {
		fprintf(stderr, "Error connecting to system bus: %s\n", error->message);
		g_clear_error(&error);
		return -ENOTCONN;
	}

	result = g_dbus_connection_call_sync(connection,
			BLUEZ_BUS_NAME,
			BLUEZ_OBJECT_PATH,
			"org.freedesktop.DBus.Introspectable",
			INTROSPECT_METHOD,
			NULL,
			G_VARIANT_TYPE("(s)"),
			G_DBUS_CALL_FLAGS_NONE,
			-1,
			NULL,
			&error);

	if (error != NULL) {
		fprintf(stderr, "Error calling Introspect method: %s\n", error->message);

		if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT))
			ret = ETIMEDOUT;
		else if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
			ret = ENOENT;
		else
			ret = EIO;

		g_clear_error(&error);
		g_object_unref(connection);
		return -ret;
	}

	g_variant_get(result, "(&s)", &xml_data);

	doc = xmlReadMemory(xml_data, strlen(xml_data), "noname.xml", NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse XML\n");
		g_variant_unref(result);
		g_object_unref(connection);
		return -EINVAL;
	}

	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	printf("Introspect XML (Pretty-printed):\n%s\n", (char *)xmlbuff);

	xmlFree(xmlbuff);
	xmlFreeDoc(doc);
	g_variant_unref(result);
	g_object_unref(connection);
	xmlCleanupParser();

	return ret;
}
