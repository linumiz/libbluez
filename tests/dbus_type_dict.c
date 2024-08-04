#include <gio/gio.h>
#include <stdio.h>

int main(void)
{
	GVariantBuilder builder;
	GVariant *dict;
	gchar *str;
	GVariantIter dict_iter;
	gint32 key;
	const gchar *value;

	/* Initialize the builder for a dictionary with key type 'i' and value type 's' */
	g_variant_builder_init(&builder, G_VARIANT_TYPE("a{is}"));

	/* Add key-value pairs to the dictionary */
	g_variant_builder_add(&builder, "{is}", 1, "one");
	g_variant_builder_add(&builder, "{is}", 2, "two");

	/* End the builder to create the dictionary GVariant */
	dict = g_variant_builder_end(&builder);

	str = g_variant_print(dict, TRUE);
	g_print("Created dictionary: %s\n", str);

	/* Check the signature */
	if (!g_variant_is_of_type(dict, G_VARIANT_TYPE("a{is}"))) {
		g_print("The GVariant is not of type 'a{is}'\n");
		g_variant_unref(dict);
		g_free(str);
		return 1;
	}

	/* Initialize the iterator for parsing the dictionary */
	g_variant_iter_init(&dict_iter, dict);

	/* Parse and print key-value pairs in a loop */
	while (g_variant_iter_next(&dict_iter, "{is}", &key, &value)) {
		g_print("Parsed key-value pair: %d -> %s\n", key, value);
	}

	g_variant_unref(dict);
	g_free(str);

	return 0;
}
