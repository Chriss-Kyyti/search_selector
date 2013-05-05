<?php

// $Id$

/**
 * @file
 * Search selector settings form.
 *
 * Last modified: 04.08.2011 Mika Hatakka
 */

/**
 * Set fields for module settings, shown to administrator.
 *
 * @return array field description for settings
 */
function search_selector_admin_settings() {
  define('VALI', '&nbsp;&nbsp;&nbsp;');
  $form = array();
  $form['standard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Included standard modules')
  );
  // Options list must start at 1, if starts at 0, the first one can't chosen
  if (module_exists('search')) {
    $std_list['1'] = t('Search from Content Management System');
  }
  if (module_exists('libdbsearch')) {
    $std_list['2'] = t('Library database search');
  }
  $std_list['3'] = 'Google';
  $ssel_path = 'admin/config/search/search_selector/';
  $ssel_targets = variable_get('stdmodules', array(1 => 1, 2 => 2, 3 => 3));
  foreach ($std_list as $key => $val) {
    $scapt = variable_get('ssel_capt_' . $key, '');
    if (!empty($scapt)) {
      $val = $scapt . ' (' . $val . ')';
      $std_list[$key] = $scapt;
    }
    if (empty($ssel_targets[$key])) {
      unset($std_list[$key]);
      if ($key == variable_get('ssel_default', 1)) {
        variable_del('ssel_default');
      }
    }
    $options[$key] = $val . VALI . l('Change caption', $ssel_path . $key . '/edit');
  }
  $form['standard']['stdmodules'] = array(
    //'#title' => t('Include standard modules'),
    '#type' => 'checkboxes',
    '#description' => (t('Select needed standard modules for the Search Selector.')),
    '#options' => $options,
    '#default_value' => $ssel_targets,
  );
  // Optional module section
  $form['optional'] = array(
    '#type' => 'fieldset',
    '#title' => t('Included optional modules')
  );
  $opt_mods = variable_get('ssel_targets', array());
  if (!empty($opt_mods)) {
    $form['optional']['opttargets'] = array(
      '#type' => 'checkboxes',
      '#description' => (t('Chosen optional modules for the Search Selector.')),
      '#options' => $opt_mods,
    );
    $form['optional']['remove'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#submit' => array('search_selector_remove_submit'),
    );
    $form['optional']['removedesc'] = array(
      '#prefix' => '<div class="description">',
      '#value' => t('Remove chosen modules from the Search Selector.'),
      '#suffix' => '</div>',
    );
  }
  else {
    $form['optional']['nomods'] = array(
      '#prefix' => '<div>',
      '#markup' => t('Optional modules have not been added to the Search Selector.'),
      '#suffix' => '</div>',
    );
  }
  foreach ($opt_mods as $key => $val) {
    if ($key > 0 && $key < 4) {
      continue;
    }
    $std_list[$key] = $val;
  }
  $form['ssel_default'] = array(
    '#title' => t('Default search target'),
    '#type' => 'select',
    '#description' => (t('Target that is selected in dropdown list.')),
    '#options' => $std_list,
    '#default_value' => variable_get('ssel_default', 1),
  );
  $form['sel_box_orientation'] = array(
    '#type' => 'radios',
    '#title' => t('Searc box orientation'),
    '#options' => array(
      t('Horizontal'),
      t('Vertical')
    ),
    '#description' => t('Set the search box orientation, horizontal or vertical.'),
    '#default_value' => variable_get('sel_box_orientation', 1),
  );
  return system_settings_form($form);
}

/**
 * Change the showing caption
 */
function search_selector_admin_edit($form, &$form_state, $selection) {
  if ($selection == '1') {
    $str = t('Search from Content Management System');
  }
  else if ($selection == '2') {
    $str = t('Library database search');
  }
  else if ($selection == '3') {
    $str = 'Google';
  }
  else {
    $str = '';
  }
  $str = t('New caption for option') . " '" . $str . "'";
  $form['caption'] = array(
    '#title' => $str,
    '#type' => 'textfield',
    '#size' => 30,
    '#default_value' => variable_get('ssel_capt_' . $selection, ''),
    '#description' => (t('Text to be shown in the Search Selector dropdown list.')),
  );
  $form['selection'] = array(
    '#type' => 'value',
    '#value' => $selection,
  );
  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('search_selector_edit_submit'),
  );
  return $form;
}

/**
 * Implementation for editing caption
 */
function search_selector_edit_submit($form, &$form_state) {
  $value = $form_state['values']['caption'];
  $selection = $form_state['values']['selection'];
  if (!empty($value) && !empty($selection)) {
    variable_set('ssel_capt_' . $selection, check_plain($value));
  }
  $form_state['redirect'] = 'admin/config/search/search_selector';
}

/**
 * Form creation form adding optional modules
 */
function search_selector_admin_add() {
  $form['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a new module to add')
  );
  $form['add']['target'] = array(
    '#title' => t('Caption for search target'),
    '#type' => 'textfield',
    '#size' => 30,
    '#description' => (t('Text to be shown in the Search Selector dropdown list.')),
  );
  $form['add']['path'] = array(
    '#title' => t('Search path'),
    '#type' => 'textfield',
    '#size' => 30,
    '#description' => (t('Path where to send search query. Mark search word with %. If outside Drupal site, start with "http://".')),
  );
  $form['add']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Add target'),
    '#submit' => array('search_selector_add_submit'),
  );

  return $form;
}

/**
 * Implementation for adding optional modules to selector
 */
function search_selector_add_submit($form, &$form_state) {
  $target = $form_state['values']['target'];
  $path = $form_state['values']['path'];
  if (!empty($target) && !empty($path)) {
    $target_list = variable_get('ssel_targets', '');
    $target_list[$path] = $target;
    variable_set('ssel_targets', $target_list);
    $form_state['redirect'] = 'admin/config/search/search_selector';
  }
}

/**
 * Implementation for removing optional modules from selector
 */
function search_selector_remove_submit($form, &$form_state) {
  $targets = variable_get('ssel_targets', '');
  foreach ($form_state['values']['opttargets'] as $val) {
    if ($val != '0') {
      unset($targets[$val]);
      variable_set('ssel_targets', $targets);
    }
  }
}