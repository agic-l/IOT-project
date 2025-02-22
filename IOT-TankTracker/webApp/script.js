// Import the functions you need from the SDKs
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.3.1/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/11.3.1/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "",
  authDomain: "",
  databaseURL: "",
  projectId: "",
  storageBucket: "",
  messagingSenderId: "",
  appId: ""
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Function to update water level
function updateWaterLevel(level) {
  const waterLevelElement = document.getElementById("waterLevel");
  waterLevelElement.textContent = `${level.toFixed(2)} cm`;
}

// Function to update tank status
function updateTankStatus(status) {
  const tankStatusElement = document.getElementById("tankStatus");
  tankStatusElement.textContent = status;
}

// Listen for changes in the "water_level" data
const waterLevelRef = ref(database, "water_level");
onValue(waterLevelRef, (snapshot) => {
  const level = snapshot.val();
  if (level !== null) {
    updateWaterLevel(level);
  }
});

// Listen for changes in the "water_status" data
const waterStatusRef = ref(database, "water_status");
onValue(waterStatusRef, (snapshot) => {
  const status = snapshot.val();
  if (status !== null) {
    updateTankStatus(status);
  }
});

console.log("Firebase listeners initialized.");
