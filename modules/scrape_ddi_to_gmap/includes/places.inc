<?php
/**
 * @file
 * Callback for search.
 */

/**
 * Callback to return search results as GeoJSON-like structure.
 *
 * @param array $bad_coords
 *   Supposed coordinates to lookup.
 *
 * @return array
 *   An array of GeoJSON "Feature" objects.
 */
function scrape_ddi_to_gmap_search_fallback(array $bad_coords) {
  return call_user_func_array('array_merge', array_map('_scrape_ddi_to_gmap_lookup', $bad_coords));
}

/**
 * Helper to perform the lookup proper.
 *
 * @param string $coord
 *   The coordinate to query.
 *
 * @return array
 *   An array of GeoJSON "Feature" objects.
 */
function _scrape_ddi_to_gmap_lookup($coord) {
  $response = drupal_http_request(url('https://maps.googleapis.com/maps/api/place/textsearch/json', array(
    'query' => array(
      'key' => variable_get('islandora_gmap_server_key'),
      'query' => $coord,
    ),
    'external' => TRUE,
  )));

  $response_data = drupal_json_decode($response->data);
  $map_to_geojson = function ($result) {
    return array(
      'type' => 'Feature',
      'geometry' => array(
        'type' => 'Point',
        'coordinates' => array(
          $result['geometry']['location']['lng'],
          $result['geometry']['location']['lat'],
        ),
      ),
    );
  };

  return array_map($map_to_geojson, $response_data['results']);
}
