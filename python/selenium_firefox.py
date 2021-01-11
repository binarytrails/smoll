# @author Vsevolod Ivanov
from selenium import webdriver

capabilities = webdriver.DesiredCapabilities.FIREFOX
capabilities['marionette'] = True
capabilities['binary'] = '/usr/bin/firefox'
options = webdriver.FirefoxOptions()
options.add_argument('--private-window')
driver = webdriver.Firefox(capabilities=capabilities, options=options)

driver.get('http://google.com')
print(driver.title)
input('press enter to continue..')

driver.quit()
