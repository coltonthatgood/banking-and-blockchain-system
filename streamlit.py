import streamlit as st
import requests

BASE_URL = "http://localhost:8080"  # your C++ blockchain server

st.set_page_config(page_title="Blockchain CLI", layout="centered")

# --- Persistent session state (acts like variables in your C++ loop)
if "users" not in st.session_state:
    st.session_state.users = []
if "current_user" not in st.session_state:
    st.session_state.current_user = None
if "show_wallet_menu" not in st.session_state:
    st.session_state.show_wallet_menu = False

def is_float(s):
    try:
        float(s)
        return True
    except ValueError:
        return False #check for float. will use later
    
# --- Main Menu
st.title("💰 Simple Blockchain Interface")

if not st.session_state.show_wallet_menu:
    st.subheader("User Menu")
    choice = st.radio(
        "Choose an option:",
        ["Create new wallet", "Select existing wallet", "View blockchain"],
        horizontal=False,
    )

    if choice == "Create new wallet":
        username = st.text_input("Enter new username:")
        if st.button("Create Wallet"):
            res = requests.post(f"{BASE_URL}/create_wallet", json={"username": username})
            if res.status_code == 200:
                data = res.json()
                st.success(f"Wallet created for {username}")
                st.code(f"Public Key: {data['publicKey']}\nPrivate Key: {data['privateKey']}")
                st.session_state.users.append(data)
            else:
                st.error("Failed to create wallet")

    elif choice == "Select existing wallet":
        if not st.session_state.users:
            st.warning("No wallets found! Please create one first.")
        else:
            usernames = [u["publicKey"] for u in st.session_state.users]
            selected = st.selectbox("Select a wallet:", usernames)
            private_key = st.text_input("Enter private key:", type="password")

            if st.button("Login"):
                user = next((u for u in st.session_state.users if u["publicKey"] == selected), None)
                if user and user["privateKey"] == private_key:
                    st.session_state.current_user = user
                    st.session_state.show_wallet_menu = True
                else:
                    st.error("Invalid private key!")

    elif choice == "View blockchain":
        res = requests.get(f"{BASE_URL}/chain")
        if res.status_code == 200:
            st.json(res.json())
    else:
        st.error("Failed to load blockchain.")


if st.session_state.show_wallet_menu:
    user = st.session_state.current_user
    st.header(f"🔑 Wallet Menu - {user['publicKey']}")

    choice = st.radio(
        "Select an option:",
        ["View Blockchain", "Mine Block","Auto-Mine Block", "Check Balance", "Transfer", "Back"],
        horizontal=False,
    )

    if choice == "View Blockchain":
        res = requests.get(f"{BASE_URL}/chain")
        if res.status_code == 200:
            st.json(res.json())

    elif choice == "Mine Block":
        #private_key = st.text_input("Enter private key:", type="password")
        pubKey = st.text_input("Enter public key:", type="password")
        prevHash = st.text_input("Enter previous hash of the block:", type="password")
        currHash = st.text_input("Enter current hash of the block:", type="password")
        if st.button("Mine Block"):
            res = requests.post(
                f"{BASE_URL}/mine",
                json={
                    "publicKey": int(pubKey) if pubKey.isdigit() else pubKey,
                    "prevHash": prevHash,
                    "currHash": currHash,
                },
            )

            if res.status_code == 200:
                data = res.json()
                if data.get("status") == "mined":
                    st.success("✅ Block mined successfully!")
                else:
                    st.warning(f"⚠️ Mining failed: {data.get('reason', 'Unknown reason')}")
            else:
                st.error(f"❌ Server error: {res.text}")
    
    elif choice == "Auto-Mine Block":
        pubKey = st.text_input("Enter Public Key:")
        if st.button("Auto-Mine"):
            res = requests.post(f"{BASE_URL}/automine", json={"publicKey": int(pubKey)})
            if res.status_code == 200:
                data = res.json()
                st.success(f"🤖 Auto block mined!\nPrev: {data['prevHash']}\nCurr: {data['currHash']}")
            else:
                st.error(f"Error: {res.text}")
    
    elif choice == "Transfer":
        #private_key = st.text_input("Enter private key:", type="password")
        senderPublicKey = user['publicKey']
        receiverPublicKey = st.text_input("Enter receiver public key:", type="password")
        Amount = st.text_input("Enter amount:", type="password")
        if st.button("Transfer"):
            res = requests.post(
                f"{BASE_URL}/transfer",
                json={
                    "senderPublicKey": int(senderPublicKey) if senderPublicKey.isdigit() else senderPublicKey,
                    "receiverPublicKey": int(receiverPublicKey) if receiverPublicKey.isdigit() else receiverPublicKey,
                    "Amount": float(Amount) if is_float(Amount) else Amount,
                },
            )

            if res.status_code == 200:
                data = res.json()
                if data.get("status") == "transferred":
                    st.success("✅ Transferred successfully!")
                else:
                    st.warning(f"⚠️ Transferring failed: {data.get('reason', 'Unknown reason')}")
            else:
                st.error(f"❌ Server error: {res.text}")

    elif choice == "Check Balance":
        public_key_str = str(user["publicKey"])
        res = requests.get(f"http://localhost:8080/checkbalance?publicKey={public_key_str}")
        st.json(res.json())

    elif choice == "Back":
        st.session_state.show_wallet_menu = False
        st.session_state.current_user = None
