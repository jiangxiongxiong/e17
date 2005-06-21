#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @param title: the text to place in the combo
 * @return Returns a pointer to a new combo on success, NULL on failure.
 * @brief Create a new internal combo
 */
Ewl_Widget *
ewl_combo_new(char *title)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	combo = NEW(Ewl_Combo, 1);
	if (!combo) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	ewl_combo_init(combo, title);

	DRETURN_PTR(EWL_WIDGET(combo), DLEVEL_STABLE);
}

/**
 * @param combo: the combo to initialize
 * @param title: the string displayed in the title
 * @return Returns no value.
 * @brief Initialize an internal combo to starting values
 */
void 
ewl_combo_init(Ewl_Combo * combo, char *title)
{
	Ewl_Container *redirect;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(combo), NULL, title);
	ewl_object_fill_policy_set(EWL_OBJECT(combo), EWL_FLAG_FILL_HFILL |
			EWL_FLAG_FILL_HSHRINK);

	ewl_widget_appearance_set( EWL_WIDGET(combo), "combo" );
	ewl_widget_inherit(EWL_WIDGET(combo), "combo");

	if (EWL_MENU_ITEM(combo)->icon)
		ewl_widget_destroy(EWL_MENU_ITEM(combo)->icon);
	EWL_MENU_ITEM(combo)->icon = NULL;

	if (EWL_MENU_ITEM(combo)->text)
		ewl_widget_destroy(EWL_MENU_ITEM(combo)->text);
	EWL_MENU_ITEM(combo)->text = NULL;

	redirect = ewl_container_redirect_get( EWL_CONTAINER(combo) );
	ewl_container_redirect_set( EWL_CONTAINER(combo), NULL );

	combo->button = ewl_button_new(NULL);
	ewl_widget_appearance_set(combo->button, "button_decrement");
	ewl_object_alignment_set(EWL_OBJECT(combo->button), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(combo), combo->button);
	ewl_widget_show(combo->button);

	combo->selected = ewl_entry_new(title);
	ewl_entry_editable_set(EWL_ENTRY(combo->selected), FALSE);
	ewl_object_fill_policy_set(EWL_OBJECT(combo->selected),
				   EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_object_alignment_set(EWL_OBJECT(combo->selected),
				 EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(combo), combo->selected);
	ewl_widget_show(combo->selected);

	ewl_callback_del(EWL_WIDGET(combo), EWL_CALLBACK_SELECT,
			 ewl_menu_base_expand_cb);

	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_CLICKED);
	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_MOUSE_DOWN);
	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_MOUSE_UP);
	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_MOUSE_MOVE);
	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_SELECT);
	ewl_container_callback_nointercept(EWL_CONTAINER(combo),
					   EWL_CALLBACK_DESELECT);

	ewl_container_redirect_set(EWL_CONTAINER(combo), redirect);

	ewl_callback_append(combo->button, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_combo_expand_cb, combo);
	ewl_callback_append(EWL_WIDGET(combo), EWL_CALLBACK_CONFIGURE,
			    ewl_combo_configure_cb, NULL);
	ewl_callback_append(combo->selected, EWL_CALLBACK_VALUE_CHANGED,
			    ewl_combo_value_changed_cb, combo);

	/*
	 * Create the popup combo portion of the widget.
	 */
	combo->base.popup = ewl_floater_new(EWL_WIDGET(combo));
	ewl_widget_internal_set(combo->base.popup, TRUE);
	ewl_widget_layer_set(combo->base.popup, 1000);
	ewl_widget_appearance_set(EWL_WIDGET(combo->base.popup), "imenu");
	ewl_box_orientation_set(EWL_BOX(combo->base.popup),
				EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(combo->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(combo->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param combo: the combo to set the selected item of
 * @param item: the entry to be set selected
 * @return Returns no value
 * @brief Set the currently selected item
 */
void
ewl_combo_selected_set(Ewl_Combo *combo, Ewl_Widget *item)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("combo", combo);
	DCHECK_PARAM_PTR("item", item);

	combo->selected = item;
	ewl_callback_call_with_event_data(EWL_WIDGET(combo),
					  EWL_CALLBACK_VALUE_CHANGED, item);
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param combo: the combo to get the selcted item of
 * @return Returns the currently selected item (possibly NULL)
 * @brief Gets the currently selected item
 */
Ewl_Widget *
ewl_combo_selected_get(Ewl_Combo * combo)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("combo", combo, NULL);

	DRETURN_PTR(combo->selected, DLEVEL_STABLE);
}

void
ewl_combo_item_select_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	combo = EWL_COMBO(user_data);

	ewl_combo_selected_set(combo, w);
	ewl_widget_hide(EWL_MENU_BASE(combo)->popup);

	ewl_widget_appearance_set(combo->button, "button_decrement");
	ewl_callback_del(EWL_MENU_BASE(combo)->popbox, EWL_CALLBACK_FOCUS_OUT,
						ewl_combo_collapse_cb);
	ewl_callback_del(combo->button, EWL_CALLBACK_MOUSE_DOWN,
						ewl_combo_collapse_cb);
	ewl_callback_append(combo->button, EWL_CALLBACK_MOUSE_DOWN,
						ewl_combo_expand_cb, combo);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	combo = EWL_COMBO(w);
	/*
	 * Position the popup combo relative to the combo.
	 */
	if (EWL_MENU_ITEM(w)->inmenu) {
		ewl_floater_position_set(EWL_FLOATER(combo->base.popup),
					 CURRENT_W(w), 0);
	}
	else {
		ewl_floater_position_set(EWL_FLOATER(combo->base.popup), 0,
					 CURRENT_H(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_value_changed_cb(Ewl_Widget *w __UNUSED__, 
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Widget *cw;
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cw = EWL_WIDGET(user_data);
	combo = EWL_COMBO(user_data);

	ewl_callback_call_with_event_data(cw, EWL_CALLBACK_VALUE_CHANGED, 
					    combo->selected);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_expand_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Combo      *combo;
	Ewl_Embed      *emb;
	Ewl_Container	 *pb;
	Ewl_Widget		 *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	combo = EWL_COMBO(user_data);
	ewl_widget_appearance_set(combo->button, "button_increment");
	ewl_menu_base_expand_cb(EWL_WIDGET(combo), ev_data, NULL);

	if (!REALIZED(combo->base.popup)) {
		emb = ewl_embed_widget_find(w);
		ewl_container_child_append(EWL_CONTAINER(emb),
					   combo->base.popup); 
	}

	pb = EWL_CONTAINER(EWL_MENU_BASE(combo)->popbox);

	ewl_callback_append(EWL_WIDGET(pb), EWL_CALLBACK_FOCUS_OUT,
					ewl_combo_collapse_cb, combo);
	ewl_callback_del(combo->button, EWL_CALLBACK_MOUSE_DOWN, ewl_combo_expand_cb);
	ewl_callback_append(combo->button, EWL_CALLBACK_MOUSE_DOWN,
					ewl_combo_collapse_cb, combo);

	ecore_list_goto_first(pb->children);

	/*
	 * Set all of the items in this menu a callback to set the currently selected
	 * widget
	 */
	while ((child = ecore_list_next(pb->children))) {
		ewl_callback_del(child, EWL_CALLBACK_SELECT, ewl_combo_item_select_cb);
		ewl_callback_append(child, EWL_CALLBACK_SELECT,
					ewl_combo_item_select_cb, combo);
	}

	ewl_widget_show(combo->base.popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_combo_collapse_cb(Ewl_Widget *w __UNUSED__, 
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Combo *combo;

	DENTER_FUNCTION(DLEVEL_STABLE);

	combo = EWL_COMBO(user_data);
	ewl_widget_hide(EWL_MENU_BASE(combo)->popup);

	ewl_widget_appearance_set(combo->button, "button_decrement");
	ewl_callback_del(EWL_MENU_BASE(combo)->popbox, EWL_CALLBACK_FOCUS_OUT,
							ewl_combo_collapse_cb);
	ewl_callback_del(combo->button, EWL_CALLBACK_MOUSE_DOWN,
							ewl_combo_collapse_cb);
	ewl_callback_append(combo->button, EWL_CALLBACK_MOUSE_DOWN,
							ewl_combo_expand_cb, combo);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

