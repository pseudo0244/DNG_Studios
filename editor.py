from PyQt5.QtWidgets import QMainWindow, QLabel
from PyQt5.QtGui import QFont

class EditorWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Sheet Editor - DNG Studios")
        self.setGeometry(100, 100, 1000, 600)

        label = QLabel("🎼 Notation Editor Coming Soon...", self)
        label.setFont(QFont("Arial", 20))
        label.move(100, 100)
