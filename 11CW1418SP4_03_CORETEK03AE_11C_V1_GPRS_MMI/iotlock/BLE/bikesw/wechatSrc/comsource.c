/*
*	WeChat mpbledemo2 common source
*
*	modified  by anqiren  2014/12/02  V1.0bat
*
**/

#include <stdio.h>
#include <string.h>
#include "app_uart.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_bondmngr.h"
#include "nrf_soc.h"
#include "ble_advdata.h"
#include "ble_wechat_service.h"
#include "ble_stack_handler.h"
#include "app_button.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "ble_conn_params.h"
#include "ble_flash.h"
#include "app_error.h"
#include "led.h"
#include "mpbledemo2.h"
#include "comsource.h"

uint8_t	 															m_addl_adv_manuf_data[BLE_GAP_ADDR_LEN];
uint16_t               								m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
ble_gap_sec_params_t   								m_sec_params;                               /**< Security requirements for this application. */
ble_gap_adv_params_t  								m_adv_params;																/**< Parameters to be passed to the stack when starting advertising. */

extern data_handler 									*m_mpbledemo2_handler;

ble_wechat_t 													m_ble_wechat;
/************************************************************************************************/
/*bond the uart to STDIO*/
/*what you have to do is add this function to your code, include <stdio.h> and use the MicroLIB.*/
/*select the MicroLIB  in the 'option of project' ,in the tab of 'target' you will see it.			*/
/************************************************************************************************/
int fputc(int ch, FILE *F)
{
	app_uart_put(ch);
	return ch;
}

/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


/**@brief Function for error handling, which is called when an error has occurred. 
 * 该函数在"app_error.h"中声明，由用户自己来定义
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);
    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

/**@brief Callback function for asserts in the SoftDevice.
 * 该函数在"nrf_assert.h"中声明，由用户自己来定义
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}
/**@brief Function for handling a Bond Manager error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void bond_manager_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the GPIOTE module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}


/*****************************************************************************
* UART init
*****************************************************************************/


static void uart_event_handler(app_uart_evt_t *p_app_uart_event)
{
    uint8_t temp;
    switch(p_app_uart_event->evt_type)
    {
			case APP_UART_DATA_READY:
					while(app_uart_get(&temp) == NRF_SUCCESS )
					{
					}
					break;
			case APP_UART_FIFO_ERROR:
					break;
			case APP_UART_COMMUNICATION_ERROR:
					break;
			case APP_UART_TX_EMPTY:
					break;
			case APP_UART_DATA:
			default:
					break;
    }
}

#define UART_BUF_SIZE                        512
 void uart_init(void)
{
    uint32_t                err_code;
    app_uart_comm_params_t  params;
    memset(&params, 0, sizeof(params));
    params.rx_pin_no = RX_PIN_NUMBER;
    params.tx_pin_no = TX_PIN_NUMBER;
    params.flow_control = APP_UART_FLOW_CONTROL_DISABLED;
    params.use_parity = false;
    params.baud_rate =UART_BAUDRATE_BAUDRATE_Baud9600; 

    APP_UART_FIFO_INIT(&params,
            UART_BUF_SIZE,
            UART_BUF_SIZE,
            uart_event_handler,
						APP_IRQ_PRIORITY_LOW,
            err_code);
    APP_ERROR_CHECK(err_code);
}

void	data_handler_init(data_handler** p_data_handler, uint8_t product_type)
{
	if (*p_data_handler == NULL) 
		{
			*p_data_handler = get_handler_by_type(product_type);
		}
}

void get_mac_addr(uint8_t *p_mac_addr)
{
		uint32_t error_code;
		ble_gap_addr_t *p_mac_addr_t = (ble_gap_addr_t*)malloc(sizeof(ble_gap_addr_t));
		error_code = sd_ble_gap_address_get(p_mac_addr_t);
		APP_ERROR_CHECK(error_code);
#ifdef CATCH_LOG
		printf("\r\n error:%d",error_code);
		printf("\r\n get mac addr");
#endif
		uint8_t *d = p_mac_addr_t->addr;
		for ( uint8_t i = 6; i >0;)
	{	
		i--;
		p_mac_addr[5-i]= d[i];
		#ifdef CATCH_LOG
		printf ( ":%02x", d[i]);
		#endif
	}
	#ifdef CATCH_LOG
		putchar ( '\n' );
	#endif
	free(p_mac_addr_t);
	p_mac_addr_t = NULL;
}
/**@brief Function for the Timer initialization.
 *
* @details Initializes the timer module. This creates and starts application timers.
*/
void timers_init(void)
{
   APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
}

/**@brief Function for handling button pull events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void button_pullup_handler(uint8_t pin_no)
{
	NVIC_DisableIRQ(GPIOTE_IRQn);
	switch (pin_no)
	{
		case SYS_RESET:
		{
		#ifdef CATCH_LOG
				printf("\r\n button 0");
		#endif
				NVIC_SystemReset ();
		}
    break; 
	}
	uint32_t err_code;
	err_code = m_mpbledemo2_handler->m_data_button_handler_func(&m_ble_wechat, pin_no);
	APP_ERROR_CHECK(err_code);
	NVIC_EnableIRQ(GPIOTE_IRQn);
	return;
}

/**@brief Function for initializing the button module.
 */
static void buttons_init(void)
{
    // Configure SYS_RESET and EVAL_BOARD_BUTTON_1 as wake up buttons and also configure
    // for 'pull up' because the eval board does not have external pull up resistors connected to
    // the buttons.
    static app_button_cfg_t buttons[] =
    {
        {SYS_RESET, false, NRF_GPIO_PIN_PULLUP, button_pullup_handler},
        {MPBLEDEMO2_BUTTON_1, false, NRF_GPIO_PIN_PULLUP, button_pullup_handler}
    };
    
    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
		uint32_t err_code;
		err_code = app_button_enable(); 
		APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
		/**< LE General Discoverable Mode, BR/EDR not supported. */
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_uuid_t adv_uuids[] =
    {
        {BLE_UUID_WECHAT_SERVICE,         BLE_UUID_TYPE_BLE}
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

		ble_advdata_manuf_data_t        manuf_data;
    manuf_data.company_identifier = COMPANY_IDENTIFIER;
    manuf_data.data.size          = sizeof(m_addl_adv_manuf_data);
    manuf_data.data.p_data        = m_addl_adv_manuf_data;
    advdata.p_manuf_specific_data = &manuf_data;
		
    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                           // Undirected advertisement
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
}

/**@brief Function for starting advertising.
 */
void advertising_start(void)
{
    uint32_t err_code;
    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
	
		led_start();
#ifdef CATCH_LOG
	printf("\r\n advertising start!");
#endif
}

/*****************************************************************************
*ble_wechat_service init
*chack what happened from ble stack 
*****************************************************************************/
void on_ble_evt(ble_evt_t * p_ble_evt) {
		uint32_t        err_code;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
					m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
						led_stop();
            break;
        case BLE_GAP_EVT_DISCONNECTED:
					#ifdef CATCH_LOG
						printf("\r\nBLE_GAP_EVT_DISCONNECTED");
					#endif
            // Since we are not in a connection and have not started advertising, store bonds
            err_code = ble_bondmngr_bonded_masters_store();
            APP_ERROR_CHECK(err_code);
						m_conn_handle = BLE_CONN_HANDLE_INVALID;
            // @note Flash access may not be complete on return of this API. System attributes are now
            // stored to flash when they are updated to ensure flash access on disconnect does not
            // result in system powering off before data was successfully written.

            // Go to system-off mode, should not return from this function, wakeup will trigger
            // a reset.
						//  advertising_start();
						NVIC_SystemReset();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
					
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            {
                err_code = sd_power_system_off();    
                APP_ERROR_CHECK(err_code);
            }
            break;
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for the Bond Manager initialization.
 */
static void bond_manager_init(void)
{
    uint32_t            err_code;
    ble_bondmngr_init_t bond_init_data;

    // Initialize the Bond Manager
    bond_init_data.flash_page_num_bond     = FLASH_PAGE_BOND;
    bond_init_data.flash_page_num_sys_attr = FLASH_PAGE_SYS_ATTR;
    bond_init_data.evt_handler             = NULL;
    bond_init_data.error_handler           = bond_manager_error_handler;
    bond_init_data.bonds_delete            = BOND_DELETE;

    err_code = ble_bondmngr_init(&bond_init_data);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_bondmngr_on_ble_evt(p_ble_evt);
		ble_wechat_on_ble_evt(&m_ble_wechat, p_ble_evt, m_mpbledemo2_handler);
    ble_conn_params_on_ble_evt(p_ble_evt);
		m_mpbledemo2_handler->m_data_on_ble_evt_func(&m_ble_wechat,p_ble_evt);
    on_ble_evt(p_ble_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    BLE_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                           BLE_L2CAP_MTU_DEF,
                           ble_evt_dispatch,
                           false);
}



/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}
/**@brief Function for the GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
	
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}
/**@brief Function for initializing the services that will be used by the application.
 *
 * @details Initialize the wechat services.
 */
static void services_init(void)
{
		uint32_t err_code;
		err_code = ble_wechat_add_service(&m_ble_wechat);
		APP_ERROR_CHECK(err_code);
		err_code = ble_wechat_add_characteristics(&m_ble_wechat);
		APP_ERROR_CHECK(err_code);
}

//function for register all products
 void register_all_products(void) {
		REGISTER(mpbledemo2);
}
/**@brief  Function for placing the application in low power state while waiting for events.
 */
void wait_app_event(void)
{
    uint32_t err_code = sd_app_event_wait();
    APP_ERROR_CHECK(err_code);
}
/*****************************************************************************
* api for main
*****************************************************************************/


void resource_init(void) {

		uart_init();
	#ifdef CATCH_LOG
	printf("\r\n*******************\r\n");
		printf("\r\n uart init");
	#endif

		timers_init();
	#ifdef CATCH_LOG
		printf("\r\n timers init");
	#endif
	
		gpiote_init();
	#ifdef CATCH_LOG
		printf("\r\n gpiote init");
	#endif
	
	
	
		buttons_init();
	#ifdef CATCH_LOG
		printf("\r\n buttons init");
	#endif

	ble_stack_init();
	#ifdef CATCH_LOG
		printf("\r\n ble stack init");
	#endif
	
	get_mac_addr(m_addl_adv_manuf_data);
	#ifdef CATCH_LOG
		
	#endif
	
    bond_manager_init();
	#ifdef CATCH_LOG
		printf("\r\n bond manager initi");
	#endif
	
		register_all_products();
	#ifdef CATCH_LOG
		printf("\r\n register products!");
	#endif
	
		data_handler_init(&m_mpbledemo2_handler,	PRODUCT_TYPE_MPBLEDEMO2);
	#ifdef CATCH_LOG
		printf("\r\n data handler init!");
		
	#endif
		
		APP_ERROR_CHECK(m_mpbledemo2_handler->m_data_init_func());
	#ifdef CATCH_LOG
		printf("\r\n mpbledemo2 init");
	#endif
	
    gap_params_init();
	#ifdef CATCH_LOG
		printf("\r\n gap params init");
	#endif
	
    advertising_init();	
	#ifdef CATCH_LOG
		printf("\r\n advertising init");
	#endif
	
    services_init();
	#ifdef CATCH_LOG
		printf("\r\n services init");
	#endif
	
    conn_params_init();
	#ifdef CATCH_LOG
		printf("\r\n conn params init");
	#endif
	
    sec_params_init();
	#ifdef CATCH_LOG
		printf("\r\n sec params init");
	#endif
	
		
}

