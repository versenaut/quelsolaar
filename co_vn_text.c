#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

extern boolean co_handle_head(BInputState *input, ENode *node, float *length);

boolean co_handle_text(BInputState *input, ENode *node)
{
	float pos = 1;
	boolean output;
	output = co_handle_head(input, node, &pos);
	return output;
}
