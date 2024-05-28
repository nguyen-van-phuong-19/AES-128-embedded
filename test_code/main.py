from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
import binascii

key = b'uwkeucarscobvydv'  # Khóa
data = b'hoenalruhoenalru'  # Dữ liệu cần mã hóa

# Tạo đối tượng AES cipher với chế độ ECB
cipher = AES.new(key, AES.MODE_ECB)

# Padding dữ liệu để phù hợp với kích thước block
padded_data = pad(data, AES.block_size)

# Mã hóa dữ liệu đã padding
encrypted_data = cipher.encrypt(padded_data)

print("Dữ liệu đã mã hóa:", encrypted_data.hex())

# Giả sử chúng ta muốn giải mã
decipher = AES.new(key, AES.MODE_ECB)
padded_decrypted_data = decipher.decrypt(encrypted_data)

# Unpadding dữ liệu sau khi giải mã
decrypted_data = unpad(padded_decrypted_data, AES.block_size)

print("Dữ liệu đã giải mã:", decrypted_data)

