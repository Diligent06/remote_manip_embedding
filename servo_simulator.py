#!/usr/bin/env python3
"""
Servo Position Control Command Generator
Simulates the servo_position_control function from the STM32 code
"""

def generate_servo_command(servo_id, position, velocity):
    """
    Generate servo position control command
    
    Args:
        servo_id (int): Servo ID (0-255)
        position (int): Target position (0-65535)
        velocity (int): Movement velocity (0-65535)
    
    Returns:
        str: Hex string representation of the command
    """
    # Initialize command array: FF FF ID LEN CMD ADDR DATA... CHECKSUM
    cmd = [0xFF, 0xFF, servo_id, 0x09, 0x03, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    
    # Set position (low byte first, then high byte)
    cmd[6] = position & 0xFF
    cmd[7] = (position >> 8) & 0xFF
    
    # Time bytes are reserved (set to 00)
    cmd[8] = 0x00
    cmd[9] = 0x00
    
    # Set velocity (low byte first, then high byte)
    cmd[10] = velocity & 0xFF
    cmd[11] = (velocity >> 8) & 0xFF
    
    # Calculate checksum using BITXOR method: XOR(SUM & 0xFF, 0xFF)
    sum_bytes = 0
    for i in range(2, 12):  # Sum from index 2 to 11 (excluding checksum)
        sum_bytes += cmd[i]
    
    cmd[12] = (sum_bytes & 0xFF) ^ 0xFF
    
    # Convert to hex string
    hex_string = ' '.join([f'{byte:02X}' for byte in cmd])
    return hex_string

def main():
    """Interactive command generator"""
    print("Servo Position Control Command Generator")
    print("=" * 40)
    
    while True:
        try:
            print("\nEnter servo parameters (or 'q' to quit):")
            
            # Get user input
            servo_id_input = input("Servo ID (0-255): ").strip()
            if servo_id_input.lower() == 'q':
                break
                
            position_input = input("Position (0-65535): ").strip()
            if position_input.lower() == 'q':
                break
                
            velocity_input = input("Velocity (0-65535): ").strip()
            if velocity_input.lower() == 'q':
                break
            
            # Convert inputs
            servo_id = int(servo_id_input)
            position = int(position_input)
            velocity = int(velocity_input)
            
            # Validate inputs
            if not (0 <= servo_id <= 255):
                print("Error: Servo ID must be between 0 and 255")
                continue
            if not (0 <= position <= 65535):
                print("Error: Position must be between 0 and 65535")
                continue
            if not (0 <= velocity <= 65535):
                print("Error: Velocity must be between 0 and 65535")
                continue
            
            # Generate command
            hex_command = generate_servo_command(servo_id, position, velocity)
            
            print(f"\nGenerated Command:")
            print(f"Hex: {hex_command}")
            print(f"Length: {len(hex_command.split())} bytes")
            
            # Show breakdown
            cmd_bytes = hex_command.split()
            print(f"\nCommand Breakdown:")
            print(f"Header:    {cmd_bytes[0]} {cmd_bytes[1]}")
            print(f"ID:        {cmd_bytes[2]} ({servo_id})")
            print(f"Length:    {cmd_bytes[3]} (9 bytes)")
            print(f"Command:   {cmd_bytes[4]} (Write)")
            print(f"Address:   {cmd_bytes[5]} (0x2A)")
            print(f"Position:  {cmd_bytes[6]} {cmd_bytes[7]} ({position})")
            print(f"Time:      {cmd_bytes[8]} {cmd_bytes[9]} (Reserved - always 00)")
            print(f"Velocity:  {cmd_bytes[10]} {cmd_bytes[11]} ({velocity})")
            print(f"Checksum:  {cmd_bytes[12]}")
            
        except ValueError:
            print("Error: Please enter valid numbers")
        except KeyboardInterrupt:
            print("\nExiting...")
            break
        except Exception as e:
            print(f"Error: {e}")

if __name__ == "__main__":
    # Example usage
    print("Example commands:")
    print("ID=1, Position=1000, Velocity=1000:")
    example_cmd = generate_servo_command(1, 1000, 1000)
    print(f"  {example_cmd}")
    
    print("\nID=1, Position=2048, Velocity=500:")
    example_cmd2 = generate_servo_command(1, 2048, 500)
    print(f"  {example_cmd2}")
    
    print("\n" + "="*50)
    main()
