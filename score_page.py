from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel, QPushButton
from home import HomePage

class ScorePage(QWidget):
    def __init__(self, score=None):
        super().__init__()

        layout = QVBoxLayout()
        layout.addWidget(QLabel(f"Score Page - {score}"))

        back_button = QPushButton("Back to Home")
        back_button.clicked.connect(self.go_back)
        layout.addWidget(back_button)

        self.setLayout(layout)

    def go_back(self):
        self.window().setCentralWidget(HomePage(self.window()))
