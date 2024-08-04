#include <gio/gio.h>
#include <stdio.h>

int main(void)
{
	GVariantBuilder dict_builder;
	GVariant *dict;
	gchar *str;
	GVariantIter iter;
	const gchar *key;
	GVariant *value;
	gint32 int_value;
	const gchar *string_value;

	/* Initialize the builder for a dictionary with key type 's' and value type 'v' */
	g_variant_builder_init(&dict_builder, G_VARIANT_TYPE("a{sv}"));

	/* Add key-value pairs to the dictionary */
	g_variant_builder_add(&dict_builder, "{sv}", "key1", g_variant_new_int32(42));
	g_variant_builder_add(&dict_builder, "{sv}", "key2", g_variant_new_string("hello"));

	/* End the builder to create the dictionary GVariant */
	dict = g_variant_builder_end(&dict_builder);

	/* Print the created dictionary */
	str = g_variant_print(dict, TRUE);
	g_print("Created dictionary: %s\n", str);

	/* Check the signature */
	if (!g_variant_is_of_type(dict, G_VARIANT_TYPE("a{sv}"))) {
		g_print("The GVariant is not of type 'a{sv}'\n");
		g_variant_unref(dict);
		g_free(str);
		return 1;
	}

	/* Initialize the iterator for parsing the dictionary */
	g_variant_iter_init(&iter, dict);

	/* Parse and print key-value pairs in a loop */
	while (g_variant_iter_next(&iter, "{&sv}", &key, &value)) {
		if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT32)) {
			int_value = g_variant_get_int32(value);
			g_print("Parsed key-value pair: %s -> %d (int)\n", key, int_value);
		} else if (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING)) {
			string_value = g_variant_get_string(value, NULL);
			g_print("Parsed key-value pair: %s -> %s (string)\n", key, string_value);
		} else {
			g_print("Parsed key-value pair: %s -> Unknown type\n", key);
		}
		g_variant_unref(value);
	}

	/* Cleanup */
	g_variant_unref(dict);
	g_free(str);

	return 0;
}
