#include <Ewl.h>

/**
 * @return Returns NULL on failure, or a pointer to a new statusbar on success.
 * @brief Allocate and initialize a new statusbar widget
 */
Ewl_Widget *ewl_statusbar_new(void)
{
	Ewl_Statusbar *sb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sb = NEW(Ewl_Statusbar, 1);
	if (!sb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	
	if (!ewl_statusbar_init(sb)) {
		ewl_widget_destroy(EWL_WIDGET(sb));
		sb = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(sb), DLEVEL_STABLE);
}

void ewl_statusbar_debug_cb(Ewl_Widget *w, void *evdata, void *data)
{
	int num = 0;
	Ewl_Widget *child;
	ecore_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(w)->children))) {
		if (VISIBLE(child)) {
			printf("Placing %p\n", child);
			num++;
		}
	}
}

/**
 * @param sb: the statusbar to initialize
 * @return Returns 1 on success and 0 on failure
 * @brief Initialize a statusbar to default values
 */
int ewl_statusbar_init(Ewl_Statusbar *sb)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sb", sb, FALSE);

	w = EWL_WIDGET(sb);

	if (!ewl_box_init(EWL_BOX(sb), EWL_ORIENTATION_HORIZONTAL)) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_object_fill_policy_set(EWL_OBJECT(w),
			EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);

	ewl_widget_appearance_set(w, "statusbar");

	sb->left = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(sb), sb->left);
	ewl_widget_internal_set(EWL_WIDGET(sb->left), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(sb->left), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(sb->left);

	sb->status = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(sb), sb->status);
	ewl_widget_internal_set(EWL_WIDGET(sb->status), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(sb->status), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(sb->status);

	ewl_callback_prepend(sb->status, EWL_CALLBACK_CONFIGURE,
			    ewl_statusbar_debug_cb, NULL);

	sb->right = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(sb), sb->right);
	ewl_widget_internal_set(EWL_WIDGET(sb->right), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(sb->right), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(sb->right), EWL_FLAG_ALIGN_RIGHT);
	ewl_widget_show(sb->right);

	ewl_container_redirect_set(EWL_CONTAINER(sb),
					EWL_CONTAINER(sb->right));

	sb->stack = ecore_list_new();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param sb: the statusbar to add the message too
 * @param txt: the status message to display
 * @return Returns no value
 * @brief Displays the given status message on the statusbar
 */
void ewl_statusbar_push(Ewl_Statusbar *sb, char *txt)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sb", sb);
	DCHECK_PARAM_PTR("txt", txt);

	if (sb->current) {
		ewl_widget_hide(sb->current);
		// ewl_container_child_remove(EWL_CONTAINER(sb->status), sb->current);
	}

	sb->current = ewl_text_new(txt);
	ewl_container_child_append(EWL_CONTAINER(sb->status), sb->current);
	ewl_widget_show(sb->current);

	ecore_list_prepend(sb->stack, sb->current);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param sb: the status bar to remove the status mesasge from
 * @return Returns no value
 * @brief Removes the most recient status message and displays the one
 * before it.
 */
void ewl_statusbar_pop(Ewl_Statusbar *sb)
{
	Ewl_Widget *current;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("sb", sb);

	current = ecore_list_remove_first(sb->stack);
	if (current) {
		ewl_widget_destroy(current);
		printf("Popped %p\n", current);
	}

	current = ecore_list_goto_first(sb->stack);
	if (current) {
		printf("\tShowing %p\n", current);
		// ewl_container_child_append(EWL_CONTAINER(sb->status), current);
		ewl_widget_show(current);
	}
	sb->current = current;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


