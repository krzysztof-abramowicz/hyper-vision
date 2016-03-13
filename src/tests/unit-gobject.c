/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Unit tests
 *     content: Test cases for GType/GObject registration (text/x-c)
 *     toolset: ISO C '99 + glib
 *     version: 1.0
 */

#include "unit-gobject.h"
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 *  ❱ Test 1: GType fundamental class
 */

/* Implementation of public virtual methods */
void hv_complex_print(HvComplex *self) {
	HV_COMPLEX_GET_CLASS(self)->print(self);
}
void hv_complex_print_impl(HvComplex *self) {
	g_print("\tRe:%5.2f Im:%5.2f\n", self->m_Re, self->m_Im);
}

/* Implementation of public non-virtual methods */
void hv_complex_set(HvComplex *self, gfloat re, gfloat im) {
	self->m_Re = re;
	self->m_Im = im;
}

/* Implementation of the class handling infrastructure */
void hv_complex_base_init(gpointer g_class) {
	if (g_test_slow()) g_print("\tHvComplex: base init\n");
}

void hv_complex_base_final(gpointer g_class) {
	if (g_test_slow()) g_print("\tHvComplex: base final\n");
}

void hv_complex_class_init(gpointer g_class, gpointer class_data) {
	HvComplexClass *this_class = HV_COMPLEX_CLASS(g_class);
	this_class->print = &hv_complex_print_impl;
	if (g_test_slow()) g_print("\tHvComplex: class init\n");
}

void hv_complex_class_final(gpointer g_class, gpointer class_data) {
	if (g_test_slow()) g_print("\tHvComplex: class final\n");
}

void hv_complex_instance_init(GTypeInstance *instance, gpointer g_class) {
	HvComplex *this_object = HV_COMPLEX(instance);
	this_object->m_Re = 0.0f;
	this_object->m_Im = 0.0f;
	if (g_test_slow()) g_print("\tHvComplex: instance init\n");
}

GType hv_complex_get_type() {
	static GType type = 0;
	if (type == 0) {
		const GTypeInfo type_info =  {
			// Type (◰size, ⇴initialise, ⇴finalise):
			(guint16) sizeof(HvComplexClass),
			(GBaseInitFunc) hv_complex_base_init,
			(GBaseFinalizeFunc) hv_complex_base_final,
			// Class (⇴initialise, ⇴finalise, →private data):
			(GClassInitFunc) hv_complex_class_init,
			(GClassFinalizeFunc) hv_complex_class_final,
			(gconstpointer) NULL,
			// Instance (◰size, #preallocations, ⇴initialise, →GType vtable):
			(guint16) sizeof(HvComplex),
			(guint16) 0,
			(GInstanceInitFunc) hv_complex_instance_init,
			// Property (not suposed)
			(GTypeValueTable *) NULL
		};
		const GTypeFundamentalInfo fundamental_info = {
			G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE
		};
		// Register type (☆GTypeID, $name, →type_info, →fundamental_info, ⚐flags)
		type = g_type_register_fundamental(
			g_type_fundamental_next(), "HvComplexType", &type_info, &fundamental_info, 0
		);
	}
	return	type;
}

/*
 *  ❯ Test 2: GObject-derived class
 */

G_DEFINE_TYPE(HvObject, hv_object, G_TYPE_OBJECT)

//: G_DEFINE_TYPE defines hv_object_get_type() function hv_object_parent_class pointer 

enum {
	OBJECT_PROPERTY_A = 1 << 1,
	OBJECT_PROPERTY_B = 1 << 2
};

/* Implementation of the constructor */
static GObject *hv_object_constructor(GType type, guint n_properties, GObjectConstructParam *properties) {
	return G_OBJECT_CLASS(hv_object_parent_class)->constructor(type, n_properties, properties);
}

/* Implementation of the destructor (dispose & finalize)*/
void hv_object_dispose(GObject *self) {
	static gboolean first_run = TRUE;
	if (first_run) {
		first_run = FALSE;
		// call g_object_unref on any GObjects held (don't break the object)
		G_OBJECT_CLASS(hv_object_parent_class)->dispose(self);
	}
}
void hv_object_finalize(GObject *self) {
	// free any memory, close any files
	G_OBJECT_CLASS(hv_object_parent_class)->finalize(self);
}

/* These are the GObject Get and Set Property methods. Their signatures are determined in gobject.h. */
void hv_object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
	HvObject *self = HV_OBJECT(object);
	switch (property_id) {
		case OBJECT_PROPERTY_A:
			g_value_set_int (value, self->a);
			break;

		case OBJECT_PROPERTY_B:
			g_value_set_float (value, self->b);
			break;
	}
}

void hv_object_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
	HvObject *self = HV_OBJECT (object);
	switch (property_id) {
		case OBJECT_PROPERTY_A:
			self-> a = g_value_get_int (value);
			break;

		case OBJECT_PROPERTY_B:
			self-> b = g_value_get_float (value);
			break;
	}
}

void hv_object_class_init(HvObjectClass *klass) {
	// override virtual methods
	GObjectClass *cls = G_OBJECT_CLASS(klass);
	cls->constructor  = &hv_object_constructor;
	cls->dispose      = &hv_object_dispose;
	cls->finalize     = &hv_object_finalize;
	cls->set_property = &hv_object_set_property;
	cls->get_property = &hv_object_get_property;
	// install properties (name, nickname, description, min, max, default, flags)
	g_object_class_install_property(cls, OBJECT_PROPERTY_A,
		g_param_spec_int("property-a", "a", "Mysterty value 1", 5, 10, 5, G_PARAM_READABLE | G_PARAM_WRITABLE)
	);
	g_object_class_install_property (cls, OBJECT_PROPERTY_B,
		g_param_spec_float("property-b", "b", "Mysterty value 2", 0.0f, 1.0f, 0.5f, G_PARAM_READABLE | G_PARAM_WRITABLE)
	);
}

void hv_object_init(HvObject *self) {
}

/*
 *  ❯ Test driver for the code
 */

void hv_gtype_create_tc() {
	if (g_test_slow()) g_print("\n");
	HvComplex *obj1 = NULL,
	          *obj2 = NULL;
	obj1 = HV_COMPLEX(g_type_create_instance(HV_COMPLEX_TYPE));
	obj2 = HV_COMPLEX(g_type_create_instance(HV_COMPLEX_TYPE));
	g_assert(obj1 != NULL && obj2 != NULL);
	if (g_test_slow()) g_print("\t----\n");
	g_type_free_instance((GTypeInstance *)obj1);
	g_type_free_instance((GTypeInstance *)obj2);
}

void hv_gtype_access_tc() {
	if (g_test_slow()) g_print("\n");
	HvComplex *obj = NULL;
	obj = HV_COMPLEX(g_type_create_instance(HV_COMPLEX_TYPE));
	g_assert(obj != NULL);
	if (g_test_slow()) {
		hv_complex_print(obj);
		hv_complex_set(obj, 3.14f, 2.71f);
		hv_complex_print(obj);
	}
}

void hv_gobject_create_tc() {
}

void hv_gobject_access_tc() {
}

int	main(int argc, char *argv[]) {
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/hv/gtype/create", hv_gtype_create_tc);
	g_test_add_func("/hv/gtype/access", hv_gtype_access_tc);
	if (g_test_slow())
		system("setterm -term linux -regtabs 4");
	return g_test_run();
}

