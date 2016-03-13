/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Unit tests
 *     content: Declarations for GType/GObject test cases (text/x-h)
 *     toolset: ISO C'99 + glib + gobject
 *     version: 1.0
 */

/* Idiom: slow test means verbose/manual test
 *   Indeed, formatted console output yealds slower execution, but it is just
 *   a barely noticeable side effect of the primary purpose's implementation.
 */

#include <glib-object.h>

/* ❯ Test 1: GType fundamental (instantiable, classed) */

/* HvComplex's public fields (structure of instance) */
typedef struct {
	GTypeInstance _vtable;
	gfloat m_Re;
	gfloat m_Im;
} HvComplex;

/* HvComplex's public virtual methods (structure of class «vtable») */
typedef struct {
	GTypeClass _typeid;
	void (*print) (HvComplex *self);
} HvComplexClass;

/* HvComplex's public methods (function prototypes) */
void hv_complex_print(HvComplex *self);
void hv_complex_set(HvComplex *self, gfloat re, gfloat im);

/* HvComplex's conventional macros and declarations */
#define HV_COMPLEX_TYPE            (hv_complex_get_type()) // ← typeid,  ↓ dynamic_cast
#define HV_COMPLEX(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), HV_COMPLEX_TYPE, HvComplex))
#define HV_COMPLEX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), HV_COMPLEX_TYPE, HvComplexClass))
#define HV_COMPLEX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), HV_COMPLEX_TYPE, HvComplexClass))
#define HV_IS_COMPLEX(obj)         (G_TYPE_CHECK_TYPE((obj), HV_COMPLEX_TYPE))
#define HV_IS_COMPLEX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), HV_COMPLEX_TYPE))

GType hv_complex_get_type();

/*  ❯ Test 2: GObject class	 */

/* Public fields (structure of instance) */
typedef struct _HvObject HvObject;
struct _HvObject {
	GObject parent;
	gint	a;
	gfloat  b;
};

/* Public virtual methods (structure of class «vtable») */
typedef struct _HvObjectClass HvObjectClass;
struct _HvObjectClass {
	GObjectClass	parent_class;
	void (*print) (HvObject *self);
};

/* HvComplex's public methods (function prototypes) */
void hv_object_print(HvComplex *self);

#define HV_OBJECT_TYPE           (hv_object_get_type())
#define HV_OBJECT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), HV_OBJECT_TYPE, HvObject))
#define HV_OBJECT_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST ((c), HV_OBJECT_TYPE, HvObjectClass))
#define HV_IS_OBJECT(obj)        (G_TYPE_CHECK_TYPE ((obj), HV_OBJECT_TYPE))
#define HV_IS_OBJECT_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE ((c), HV_OBJECT_TYPE))
#define HV_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), HV_OBJECT_TYPE, HvObjectClass))

