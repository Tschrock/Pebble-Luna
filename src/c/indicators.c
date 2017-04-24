#include <pebble.h>
#include "indicators.h"

/**
    Pebble Indicators
    Bluetooth and Battery indicators for a pebble app/watchface using spritesheets.

    Author: Tschrock (tschrock123@gmail.com)
    Version: 1.0 6/22/15
*/

#include <pebble.h>
#include "indicators.h"
  
void initBatterySettings() {
  
  // The number of different battery icons
  batteryIcons = 12;
  
  // Weather or not to subscribe to battery change events. If you handle these events yourself be sure to call checkBattery() to update the icon.
  batterySubscribe = true;
  
  // The position of the icon on the screen
  batteryPos = GRect(123, 3, 17, 10);
  
  // The start position and size of the icon on the icon sheet
  batteryIconSize = GRect(0, 0, 17, 10);
  
  
  #ifdef PBL_COLOR
  
    // The resource id of the color icon sheet
    batteryResourceId = RESOURCE_ID_BATT;
  
  #else
  
    // The resource id of the _BLACK icon sheet
    batteryResourceId = RESOURCE_ID_BATT_BLACK;
  
    // The resource id of the _WHITE icon sheet
    batteryResourceId2 = RESOURCE_ID_BATT_WHITE;
  
  #endif
}

void initBluetoothSettings() {
  
  // Weather or not to subscribe to bluetooth change events. If you handle these events yourself be sure to call checkBluetooth() to update the icon.
  bluetoothSubscribe = true;
  
  // The position of the icon on the screen
  bluetoothPos = GRect(3, 3, 13, 12);
  
  // The start position and size of the icon on the icon sheet
  bluetoothIconSize = GRect(0, 0, 13, 12);
  
  
  #ifdef PBL_COLOR
  
    // The resource id of the color icon sheet
    bluetoothResourceId = RESOURCE_ID_BLTH;
  
  #else
  
    // The resource id of the _BLACK icon sheet
    bluetoothResourceId = RESOURCE_ID_BLTH_BLACK;
  
    // The resource id of the _WHITE icon sheet
    bluetoothResourceId2 = RESOURCE_ID_BLTH_WHITE;
  
  #endif
}

void initIndicatorSettings() {
  initBatterySettings();
  initBluetoothSettings();
}

//
// Spritesheet functions
//

GBitmap* getIconFromSheet(const GBitmap * iconSheet, const int xIndex, const int yIndex, const GRect iconSize) {
  return gbitmap_create_as_sub_bitmap(iconSheet, GRect(iconSize.origin.x + (xIndex * iconSize.size.w), iconSize.origin.y + (yIndex * iconSize.size.h), iconSize.size.w, iconSize.size.h));
}

void swapSubBitmap(GBitmap ** oldBitmapPtr, GBitmap * newBitmap, BitmapLayer * bitmapLayer) {
  bitmap_layer_set_bitmap(bitmapLayer, newBitmap);  
  gbitmap_destroy(*oldBitmapPtr);
  *oldBitmapPtr = newBitmap;
}


//
// Battery Update functions
//

void checkBattery_(BatteryChargeState batState) {
  
  int index = batState.is_charging ? batteryIcons - 1 : (batState.charge_percent / 100.0) * (batteryIcons - 2);
  swapSubBitmap(&s_battery_subbitmap, getIconFromSheet(s_battery_bitmap, 0, index, batteryIconSize), s_battery_layer);
  
  #ifdef PBL_BW
    swapSubBitmap(&s_battery_subbitmap2, getIconFromSheet(s_battery_bitmap2, 0, index, batteryIconSize), s_battery_layer2);
  #endif
}

void checkBattery() {
  checkBattery_(battery_state_service_peek());
}



//
// Bluetooth Update functions
//

void checkBluetooth_(bool connected) {
  swapSubBitmap(&s_bluetooth_subbitmap, getIconFromSheet(s_bluetooth_bitmap, 0, (connected ? 0 : 1), bluetoothIconSize), s_bluetooth_layer);
  
  #ifdef PBL_BW
    swapSubBitmap(&s_bluetooth_subbitmap2, getIconFromSheet(s_bluetooth_bitmap2, 0, (connected ? 0 : 1), bluetoothIconSize), s_bluetooth_layer2);
  #endif
}

void checkBluetooth() {
  checkBluetooth_(bluetooth_connection_service_peek());
}

//
// Window Setup
//

void initBatteryIcon(Window *window) {
  
  s_battery_layer = bitmap_layer_create(batteryPos);
  s_battery_bitmap = gbitmap_create_with_resource(batteryResourceId);
  s_battery_subbitmap = getIconFromSheet(s_battery_bitmap, 0, 0, batteryIconSize);
  
  bitmap_layer_set_compositing_mode(s_battery_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_battery_layer, s_battery_subbitmap);
  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_layer));
  
  #ifdef PBL_BW
    s_battery_layer2 = bitmap_layer_create(batteryPos);
    s_battery_bitmap2 = gbitmap_create_with_resource(batteryResourceId2);
    s_battery_subbitmap2 = getIconFromSheet(s_battery_bitmap2, 0, 0, batteryIconSize);
  
    bitmap_layer_set_compositing_mode(s_battery_layer, GCompOpClear);  
    bitmap_layer_set_compositing_mode(s_battery_layer2, GCompOpOr);
    bitmap_layer_set_bitmap(s_battery_layer2, s_battery_subbitmap2);
    
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_layer2));
  #endif
  
  checkBattery();
  if(batterySubscribe) {
    battery_state_service_subscribe(checkBattery_);
  }
}

void deinitBatteryIcon() {
  gbitmap_destroy(s_battery_subbitmap);
  gbitmap_destroy(s_battery_bitmap);
  bitmap_layer_destroy(s_battery_layer);
  
  #ifdef PBL_BW
    gbitmap_destroy(s_battery_subbitmap2);
    gbitmap_destroy(s_battery_bitmap2);
    bitmap_layer_destroy(s_battery_layer2);
  #endif
  
  if(batterySubscribe) {
    battery_state_service_unsubscribe();
  }
}

void initBluetoothIcon(Window *window) {
  
  s_bluetooth_layer = bitmap_layer_create(bluetoothPos);
  s_bluetooth_bitmap = gbitmap_create_with_resource(bluetoothResourceId);
  s_bluetooth_subbitmap = getIconFromSheet(s_bluetooth_bitmap, 0, 0, bluetoothIconSize);

  bitmap_layer_set_compositing_mode(s_bluetooth_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_subbitmap);
  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bluetooth_layer));
  
  #ifdef PBL_BW
    s_bluetooth_layer2 = bitmap_layer_create(bluetoothPos);
    s_bluetooth_bitmap2 = gbitmap_create_with_resource(bluetoothResourceId2);
    s_bluetooth_subbitmap2 = getIconFromSheet(s_bluetooth_bitmap2, 0, 0, bluetoothIconSize);
    
    bitmap_layer_set_compositing_mode(s_bluetooth_layer, GCompOpClear);  
    bitmap_layer_set_compositing_mode(s_bluetooth_layer2, GCompOpOr);
    bitmap_layer_set_bitmap(s_bluetooth_layer2, s_bluetooth_subbitmap2);
  
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bluetooth_layer2));
  #endif

  checkBluetooth();
  if(bluetoothSubscribe) {
    bluetooth_connection_service_subscribe(checkBluetooth_);
  }
}

void deinitBluetoothIcon() {
  gbitmap_destroy(s_bluetooth_subbitmap);
  gbitmap_destroy(s_bluetooth_bitmap);
  bitmap_layer_destroy(s_bluetooth_layer);
  
  #ifdef PBL_BW
    gbitmap_destroy(s_bluetooth_subbitmap2);
    gbitmap_destroy(s_bluetooth_bitmap2);
    bitmap_layer_destroy(s_bluetooth_layer2);
  #endif
    
  if(bluetoothSubscribe) {
    bluetooth_connection_service_unsubscribe();
  }
}
