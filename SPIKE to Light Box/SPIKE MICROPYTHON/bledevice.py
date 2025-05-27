from micropython import const
import bluetooth
import struct
import time
import micropython
micropython.alloc_emergency_exception_buf(128)

import binascii

def str_to_uuid(uuid_str):
    """Convert a UUID string to bytes."""
    uuid_str = uuid_str.replace('-', '')
    return binascii.unhexlify(uuid_str)



SERVICE_UUID = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
WRITE_UUID   = bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
NOTIFY_UUID  = bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")
#SERVICE_UUID = bluetooth.UUID(0xfd02)
#WRITE_UUID = bluetooth.UUID(0x2a05)
#NOTIFY_UUID = bluetooth.UUID(0x2b2a)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)

_UART_UUID = bluetooth.UUID(0xfd02)
_UART_TX = (NOTIFY_UUID,_FLAG_NOTIFY,)
_UART_RX = (WRITE_UUID,_FLAG_WRITE,)
_UART_SERVICE = (_UART_UUID,(_UART_TX, _UART_RX),)

# BLE IRQ event codes
_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_MTU_EXCHANGE = const(21)

mtu_size=150

class BLEDevice:
    def __init__(self):
        self.ble = bluetooth.BLE()
        self.ble.active(True)
        self.ble.config(mtu=mtu_size)
        #self.ble.gattc_exchange_mtu(185)
        #self.ble.config(rxbuf=185)

        self.conn_handle = None
        self.write_handle = None
        self.notify_handle = None
        self.service_uuid = SERVICE_UUID
        print(self.service_uuid)
        self.write_uuid = WRITE_UUID
        self.notify_uuid = NOTIFY_UUID
        self.callback = None
        self._connecting = False
        self.start_handle = None
        self.end_handle = None
        
        self.ble.irq(self._irq)
        
        #rxbuf = 185
        #((self._tx_handle, self._rx_handle),) = self.ble.gatts_register_services((_UART_SERVICE,))
        #self.ble.gatts_set_buffer(self._tx_handle, rxbuf, True)

    def _irq(self, event, data):
        """Handle BLE IRQ events"""
        if event == _IRQ_SCAN_RESULT:
            addr_type, addr, adv_type, rssi, adv_data = data
            addr_str = ':'.join('%02X' % i for i in addr)
            name = ''
            manufacturer = ''
            if adv_type == 4:
                name = self.decode(adv_data)
            if addr_str not in self.seen_addresses:
                self.seen_addresses.add(addr_str)
                manufacturer = self.decode(adv_data)
                name = self.decode(adv_data)
                device_info = {'device': addr_str,
                               'manufacture':manufacturer,
                               'name':name,
                               'rssi':rssi}
                self.devices.append(device_info)
                if (self.mfg and manufacturer == self.mfg) or (name and self.name in name):
                    print('found it', name, manufacturer)
                    self.found = True
            else:   #add a new name if you got one
                for d in self.devices:
                    if d['device'] == addr_str:
                        if name and d['name'] != name:
                            d['name'] = name
                            if self.name in name:
                                print('found it2', name, manufacturer)
                                self.found = True
            if self.found:
                # Stop scanning
                self.ble.gap_scan(None)
                # Connect to device
                self._connecting = True
                self.ble.gap_connect(addr_type, addr)
                print("Device found, connecting...")
        
        elif event == _IRQ_PERIPHERAL_CONNECT:
            # Connected successfully
            conn_handle, addr_type, addr = data
            self.conn_handle = conn_handle
            self._connecting = False
            # Start service discovery
            self.ble.gattc_discover_services(self.conn_handle)
            print("Connected")
            
        elif event == _IRQ_PERIPHERAL_DISCONNECT:
            # Disconnected
            conn_handle, addr_type, addr = data
            self.conn_handle = None
            self.write_handle = None
            self.notify_handle = None
            print("Disconnected")
            
        elif event == _IRQ_GATTC_SERVICE_RESULT:
            # Found a service
            print('services discovered')
            conn_handle, start_handle, end_handle, uuid = data
            print(uuid)
            if self.service_uuid == uuid:
                print('matches')
                self.start_handle = start_handle
                self.end_handle = end_handle
                
        elif event == _IRQ_GATTC_SERVICE_DONE:
            # Service query complete.
            if self.start_handle and self.end_handle:
                self.ble.gattc_discover_characteristics(self.conn_handle, self.start_handle, self.end_handle)
            else:
                print("Failed to find service.")
                    
        elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
            # Found a characteristic
            print('characteristics discovered')
            conn_handle, def_handle, value_handle, properties, uuid = data
            print(uuid)
            
            if uuid == self.write_uuid:
                self.write_handle = value_handle
                print("Write characteristic found")
            elif uuid == self.notify_uuid:
                self.notify_handle = value_handle
                print("Notify characteristic found")
                # Enable notifications
                self.ble.gattc_write(conn_handle, value_handle + 1, bytes([1,0]))
                self.ble.gattc_exchange_mtu(self.conn_handle)  # set larger minimum message size
                
        elif event == _IRQ_GATTS_WRITE:
            # Received data
            print('got something ',data)
            conn_handle, value_handle = data
            if conn_handle == self.conn_handle and value_handle == self.notify_handle:
                if self.callback:
                    data = self.ble.gatts_read(value_handle)
                    self.callback(data)

        elif event == _IRQ_GATTC_NOTIFY:
            #A server has sent a notify request.
            conn_handle, value_handle, notify_data = data
            if conn_handle == self.conn_handle and value_handle == self.notify_handle:
                if self.callback:
                    self.callback(notify_data)
                    
        elif event == _IRQ_MTU_EXCHANGE:
            print('increased minimum size')
            
        else:
            print('new event ',event) 

    def write(self, data):
        """Write data to the characteristic"""
        if not self.conn_handle or not self.write_handle:
            print('Not connected to device')
            return
        
        try:
            # Convert data to bytes if it isn't already
            if not isinstance(data, bytes):
                data = bytes(data)
            self.ble.gattc_write(self.conn_handle, self.write_handle, data)
            print('Sent:', data)
        except Exception as e:
            print('Error writing:', e)
    
    def is_connected(self):
        return (self.conn_handle is not None and self.notify_handle is not None and self.write_handle is not None)
    
    def set_callback(self, callback):
        """Set the callback for notifications"""
        self.callback = callback
    
    def disconnect(self):
        """Disconnect from the device"""
        if self.conn_handle is not None:
            self.ble.gap_disconnect(self.conn_handle)
            self.conn_handle = None
            self.write_handle = None
            self.notify_handle = None
            
    def decode(self, payload):
        i = 0
        while i < len(payload):
            if i + 2 > len(payload): break
            length = payload[i]
            if i + length + 1 > len(payload): break
            
            adv_type = payload[i + 1]
            if adv_type == 0xFF:  # Manufacturer Specific Data
                if length >= 3:  # Need at least 2 bytes for manufacturer ID
                    mfg_id = payload[i + 3] << 8 | payload[i + 2]
                    if mfg_id == 0x004C:
                        return "Apple"
                    elif mfg_id == 0x0006:
                        return "Microsoft"
                    else:
                        return f"Mfg 0x{mfg_id:04X}"
            if adv_type in (0x08, 0x09):  # Shortened or Complete Local Name
                name_bytes = payload[i + 2:i + length + 1]
                try:
                    return bytes(name_bytes).decode('utf-8')
                except:
                    return None
            i += length + 1
        return None
    
    def reset(self):
        self.ble.active(True)
        self.mfg = None
        self.name = None
        self.seen_addresses = set()
        self.devices = []
        self.found = False

    def scan(self, duration = 0, manufacture = None, name = None):
        print("Scanning for devices...")
        self.reset()
        self.mfg = manufacture
        self.name = name
        self.ble.gap_scan(duration, 30000, 30000, True)
    
    def close(self):
        self.ble.gap_scan(None)
        self.ble.active(False)
