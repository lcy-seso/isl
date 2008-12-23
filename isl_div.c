#include <isl_div.h>
#include <isl_map.h>

static unsigned n(struct isl_div *d, enum isl_dim_type type)
{
	struct isl_dim *dim = d->bmap->dim;
	switch (type) {
	case isl_dim_param:	return dim->nparam;
	case isl_dim_in:	return dim->n_in;
	case isl_dim_out:	return dim->n_out;
	case isl_dim_div:	return d->bmap->n_div;
	}
}

static unsigned offset(struct isl_div *d, enum isl_dim_type type)
{
	struct isl_dim *dim = d->bmap->dim;
	switch (type) {
	case isl_dim_param: return 1 + 1;
	case isl_dim_in:    return 1 + 1 + dim->nparam;
	case isl_dim_out:   return 1 + 1 + dim->nparam + dim->n_in;
	case isl_dim_div:   return 1 + 1 + dim->nparam + dim->n_in + dim->n_out;
	}
}

struct isl_div *isl_basic_map_div(struct isl_basic_map *bmap, isl_int **line)
{
	struct isl_div *div;

	if (!bmap || !line)
		goto error;
	
	div = isl_alloc_type(bmap->ctx, struct isl_div);
	if (!div)
		goto error;

	div->ctx = bmap->ctx;
	isl_ctx_ref(div->ctx);
	div->ref = 1;
	div->bmap = bmap;
	div->line = line;

	return div;
error:
	isl_basic_map_free(bmap);
	return NULL;
}

struct isl_div *isl_div_free(struct isl_div *c)
{
	if (!c)
		return;

	if (--c->ref > 0)
		return;

	isl_basic_map_free(c->bmap);
	isl_ctx_deref(c->ctx);
	free(c);
}

void isl_div_get_constant(struct isl_div *div, isl_int *v)
{
	if (!div)
		return;
	isl_int_set(*v, div->line[0][1]);
}

void isl_div_get_denominator(struct isl_div *div, isl_int *v)
{
	if (!div)
		return;
	isl_int_set(*v, div->line[0][0]);
}

void isl_div_get_coefficient(struct isl_div *div,
	enum isl_dim_type type, int pos, isl_int *v)
{
	if (!div)
		return;

	isl_assert(div->ctx, pos < n(div, type), return);
	isl_int_set(*v, div->line[0][offset(div, type) + pos]);
}
