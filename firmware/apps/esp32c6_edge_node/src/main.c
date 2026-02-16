#include <zephyr/app_version.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static void net_mgmt_event_handler(struct net_mgmt_event_callback* cb,
                                   uint64_t mgmt_event,
                                   struct net_if* iface) {
  if (mgmt_event == NET_EVENT_WIFI_CONNECT_RESULT) {
    struct wifi_status* status = (struct wifi_status*)cb->info;
    if (status->status) {
      LOG_ERR("Failed to connect to Wi-Fi: %d", status->status);
    } else {
      LOG_INF("Connected to Wi-Fi");
    }
  } else if (mgmt_event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
    struct wifi_status* status = (struct wifi_status*)cb->info;
    LOG_INF("Disconnected from Wi-Fi: reason %d", status->disconn_reason);
  } else {
    LOG_WRN("Unknown Wi-Fi event: %lld", mgmt_event);
  }
}

static int wifi_init(void) {
  struct net_if* iface = net_if_get_wifi_sta();
  if (iface == NULL) {
    LOG_ERR("Failed to get Wi-Fi interface");
    return -1;
  }

  static struct net_mgmt_event_callback wifi_cb;
  net_mgmt_init_event_callback(
      &wifi_cb, net_mgmt_event_handler,
      NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
  net_mgmt_add_event_callback(&wifi_cb);

  int rc = net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, iface, NULL, 0);
  if (rc) {
    LOG_ERR("Failed to connect to Wi-Fi: %d", rc);
    return -1;
  }
  return 0;
}

int main(void) {
  LOG_INF("Firmware version: " APP_VERSION_EXTENDED_STRING);

  if (wifi_init() != 0) {
    return -1;
  }
  while (1) {
    k_sleep(K_SECONDS(10));
  }
}