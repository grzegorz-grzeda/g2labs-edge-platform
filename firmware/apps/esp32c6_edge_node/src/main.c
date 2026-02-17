#include <zephyr/app_version.h>
#include <zephyr/autoconf.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);
#define WIFI_TIMER_TIMEOUT_SECONDS 10

static void wifi_timer_expiry_cb(struct k_timer* timer);

K_SEM_DEFINE(wifi_sem, 0, 1);
K_MUTEX_DEFINE(wifi_mutex);
K_TIMER_DEFINE(wifi_timer, wifi_timer_expiry_cb, NULL);
static bool wifi_connected = false;

static int wifi_connect_stored() {
    struct net_if* iface = net_if_get_wifi_sta();
    if (iface == NULL) {
        LOG_ERR("Failed to get Wi-Fi interface");
        return -1;
    }

    k_timer_start(&wifi_timer, K_SECONDS(WIFI_TIMER_TIMEOUT_SECONDS),
                  K_NO_WAIT);

    int rc = net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, iface, NULL, 0);
    if (rc) {
        LOG_ERR("Failed to connect to Wi-Fi: %d", rc);
        return -1;
    }
    return 0;
}

static void wifi_timer_expiry_cb(struct k_timer* timer) {
    ARG_UNUSED(timer);
    LOG_ERR("Wi-Fi connection timed out");
    wifi_connect_stored();
}

static void net_mgmt_event_handler(struct net_mgmt_event_callback* cb,
                                   uint64_t mgmt_event,
                                   struct net_if* iface) {
    ARG_UNUSED(cb);
    ARG_UNUSED(iface);

    if (mgmt_event == NET_EVENT_L4_CONNECTED) {
        LOG_INF("Network connected");
        k_timer_stop(&wifi_timer);
        k_mutex_lock(&wifi_mutex, K_FOREVER);
        wifi_connected = true;
        k_mutex_unlock(&wifi_mutex);
        k_sem_give(&wifi_sem);
    } else if (mgmt_event == NET_EVENT_L4_DISCONNECTED) {
        k_timer_stop(&wifi_timer);
        LOG_INF("Network disconnected");
        k_mutex_lock(&wifi_mutex, K_FOREVER);
        wifi_connected = false;
        k_mutex_unlock(&wifi_mutex);
    } else {
        LOG_WRN("Unknown network event: %lld", mgmt_event);
    }
}

static void wait_for_wifi() {
    LOG_INF("Waiting for Wi-Fi connection...");
    k_sem_take(&wifi_sem, K_FOREVER);
    k_mutex_lock(&wifi_mutex, K_FOREVER);
    if (wifi_connected) {
        LOG_INF("Wi-Fi is connected - mutex");
    } else {
        LOG_ERR("Wi-Fi connection failed - mutex");
    }
    k_mutex_unlock(&wifi_mutex);
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
        NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED);
    net_mgmt_add_event_callback(&wifi_cb);
    conn_mgr_mon_resend_status();

    return wifi_connect_stored();
}

int main(void) {
    LOG_INF("Firmware version: " APP_VERSION_EXTENDED_STRING);

    if (wifi_init() != 0) {
        return -1;
    }
    wait_for_wifi();

    while (1) {
        k_sleep(K_SECONDS(10));
    }
}