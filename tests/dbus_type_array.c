#include <gio/gio.h>

int main(void)
{
	gint32 values[] = {1, 2, 3, 4, 5};
	gsize n_values = sizeof(values) / sizeof(gint32);
	GVariantBuilder builder;
	GVariant *array;
	gchar *str;
	gsize i;
	GVariantIter iter;
	GVariant *child;
	gint32 value;
	gboolean is_valid_type;

	/* Create a GVariant array of type 'ai' (array of integers) */
	g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);

	for (i = 0; i < n_values; i++) {
		g_variant_builder_add(&builder, "i", values[i]);
	}

	array = g_variant_builder_end(&builder);

	/* Print the created array */
	str = g_variant_print(array, TRUE);
	g_print("Created array: %s\n", str);

	/* Check the signature */
	is_valid_type = g_variant_is_of_type(array, G_VARIANT_TYPE("ai"));
	if (!is_valid_type) {
		g_print("The GVariant is not of type 'ai'\n");
		g_variant_unref(array);
		g_free(str);
		return 1;
	}

	/* Initialize the iterator for parsing */
	g_variant_iter_init(&iter, array);

	/* Parse and print values in a loop */
	while ((child = g_variant_iter_next_value(&iter)) != NULL) {
		g_variant_get(child, "i", &value);
		g_print("Parsed value: %d\n", value);
		g_variant_unref(child);
	}

	g_variant_unref(array);
	g_free(str);

	return 0;
}
