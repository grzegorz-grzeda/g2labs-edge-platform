#include <zephyr/app_version.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

int main(void) {
  LOG_INF("Hello from ESP32-C6 Edge Node!");
  LOG_INF("Firmware version: " APP_VERSION_EXTENDED_STRING);
  struct net_if* iface = net_if_get_wifi_sta();
  if (iface == NULL) {
    LOG_ERR("Failed to get Wi-Fi interface");
    return -1;
  }

  int rc = net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, iface, NULL, 0);
  if (rc) {
    LOG_ERR("Failed to connect to Wi-Fi: %d", rc);
    return -1;
  }
  while (1) {
    k_sleep(K_SECONDS(10));
  }
}