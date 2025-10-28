from micropython import const
import math
import bluetooth
from ble_advertising import advertising_payload
import struct
import time

# Constants
RSSI_MEASURED_POWER = const(-69)  # Calibrated RSSI at 1 meter distance
ENVIRONMENTAL_FACTOR = const(2.0)  # Path loss exponent (2.0 for free space)
SAMPLES_COUNT = const(5)  # Number of RSSI samples to average
SAMPLE_INTERVAL = const(100)  # Time between samples in ms

class DistanceEstimator:
    def __init__(self, ble):
        self._ble = ble
        self._ble.active(True)
        self._ble.irq(self._irq)
        self._reset()
        
        # Advertisement data
        self._payload = advertising_payload(
            name="NRF_DISTANCE",
            services=[bluetooth.UUID(0x181A)],  # Environmental Sensing Service
        )
        
    def _reset(self):
        self._rssi_samples = []
        self._addr_to_rssi = {}
        self._scanning = False
        self._advertising = False
    
    def _irq(self, event, data):
        if event == bluetooth.IRQ_SCAN_RESULT:
            addr_type, addr, adv_type, rssi, adv_data = data
            addr_str = bytes(addr).hex()
            if addr_str not in self._addr_to_rssi:
                self._addr_to_rssi[addr_str] = []
            self._addr_to_rssi[addr_str].append(rssi)
    
    def start_advertising(self):
        """Start advertising the device."""
        self._ble.gap_advertise(100000, adv_data=self._payload)
        self._advertising = True
    
    def stop_advertising(self):
        """Stop advertising the device."""
        self._ble.gap_advertise(None)
        self._advertising = False
    
    def start_scanning(self):
        """Start scanning for other devices."""
        self._ble.gap_scan(0, 30000, 30000)
        self._scanning = True
    
    def stop_scanning(self):
        """Stop scanning for other devices."""
        self._ble.gap_scan(None)
        self._scanning = False
        
    def collect_rssi_samples(self, target_addr=None, duration_ms=SAMPLES_COUNT * SAMPLE_INTERVAL):
        """
        Collect RSSI samples from a specific device or all detected devices.
        
        Args:
            target_addr: Optional hex string of target device address
            duration_ms: Duration to collect samples in milliseconds
        
        Returns:
            Dictionary of device addresses to list of RSSI values
        """
        self._addr_to_rssi.clear()
        self.start_scanning()
        
        start_time = time.ticks_ms()
        while time.ticks_diff(time.ticks_ms(), start_time) < duration_ms:
            time.sleep_ms(SAMPLE_INTERVAL)
            
        self.stop_scanning()
        
        if target_addr:
            return {target_addr: self._addr_to_rssi.get(target_addr, [])}
        return self._addr_to_rssi
    
    def estimate_distance(self, rssi_samples):
        """
        Estimate distance based on RSSI samples using the log-distance path loss model.
        
        Args:
            rssi_samples: List of RSSI values
            
        Returns:
            Estimated distance in meters, or None if insufficient samples
        """
        if not rssi_samples or len(rssi_samples) < SAMPLES_COUNT:
            return None
            
        # Average the RSSI samples
        avg_rssi = sum(rssi_samples) / len(rssi_samples)
        
        # Calculate distance using log-distance path loss model
        # distance = 10 ^ ((Measured Power - RSSI) / (10 * Path Loss Exponent))
        try:
            distance = math.pow(10, (RSSI_MEASURED_POWER - avg_rssi) / (10 * ENVIRONMENTAL_FACTOR))
            return round(distance, 2)
        except Exception:
            return None
    
    def get_distances(self):
        """
        Get estimated distances to all detected devices.
        
        Returns:
            Dictionary of device addresses to estimated distances in meters
        """
        distances = {}
        for addr, samples in self._addr_to_rssi.items():
            distance = self.estimate_distance(samples)
            if distance is not None:
                distances[addr] = distance
        return distances


import bluetooth
from nrf_distance import DistanceEstimator

# Initialize Bluetooth
ble = bluetooth.BLE()
estimator = DistanceEstimator(ble)

# Start advertising this device
estimator.start_advertising()

# Collect RSSI samples and estimate distances
samples = estimator.collect_rssi_samples()
distances = estimator.get_distances()

for addr, distance in distances.items():
    print(f"Device {addr}: {distance}m")
