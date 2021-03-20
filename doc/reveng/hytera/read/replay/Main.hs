module Main where

import Numeric
import Data.Foldable.Extra
import System.Exit
import qualified Data.ByteString as BS
import Data.Text (split, pack, unpack)
import System.Environment
import System.USB.Initialization
import System.USB.DeviceHandling
import System.USB.Enumeration
import System.USB.Descriptors
import System.USB.IO
import Data.List (intersperse, intercalate)

isHytera :: Device -> IO Bool
isHytera dev = do
  desc <- getDeviceDesc dev
  return $ (deviceVendorId desc == 0x238b) && (deviceProductId desc == 0x0a11)

findDevice :: Ctx -> IO DeviceHandle
findDevice ctx = do
  devices <- getDevices ctx
  device <- findM isHytera devices
  case device of
    Just dev -> openDevice dev
    Nothing -> do
      putStrLn "Error: Device not found!"
      exitFailure

prettyPrint :: BS.ByteString -> String
prettyPrint = unwords . map (`showHex` "") . BS.unpack

parsePacket :: String -> BS.ByteString
parsePacket pkt = BS.pack $ fst . head . readHex . unpack <$> (split isDelim . pack $ pkt)
  where isDelim x = x == ':'

readPackets :: IO [String]
readPackets = getArgs >>= readFile . head >>= pure . lines

sendPacket :: DeviceHandle -> BS.ByteString -> IO ()
sendPacket d pkt = do
  let epOut = EndpointAddress 4 Out
      epIn  = EndpointAddress 4 In
  _ <- writeBulk d epOut pkt 1024
  (response, _) <- readBulk d epIn 1500 10
  putStrLn $ intercalate " -> " $ show . BS.length <$> [pkt, response]

main :: IO ()
main = do
  packets <- readPackets
  usbCtx <- newCtx
  d <- findDevice usbCtx
  putStrLn "Found device:"
  print d
  sequence_ $ sendPacket d . parsePacket <$> packets
