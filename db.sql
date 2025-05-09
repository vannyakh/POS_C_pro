-- Database Schema for Restaurant POS System

-- Staff Table: Stores information about employees
CREATE TABLE staff (
    staff_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,  -- In production, use proper password hashing
    name TEXT NOT NULL,
    role TEXT NOT NULL,      -- Manager, Waiter, Cashier, etc.
    contact TEXT,
    is_active INTEGER DEFAULT 1,  -- Boolean: 1 for active, 0 for inactive
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Menu Categories Table
CREATE TABLE menu_categories (
    category_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    description TEXT
);

-- Menu Items Table: For food and beverage items
CREATE TABLE menu_items (
    item_id INTEGER PRIMARY KEY AUTOINCREMENT,
    category_id INTEGER,
    name TEXT NOT NULL,
    description TEXT,
    price REAL NOT NULL,
    is_available INTEGER DEFAULT 1,  -- Boolean: 1 for available, 0 for unavailable
    FOREIGN KEY (category_id) REFERENCES menu_categories (category_id)
);

-- Tables in the Restaurant
CREATE TABLE restaurant_tables (
    table_id INTEGER PRIMARY KEY AUTOINCREMENT,
    table_number TEXT UNIQUE NOT NULL,
    capacity INTEGER,
    status TEXT DEFAULT 'available'  -- available, occupied, reserved
);

-- Orders Master Table
CREATE TABLE orders (
    order_id INTEGER PRIMARY KEY AUTOINCREMENT,
    table_id INTEGER,
    staff_id INTEGER,
    customer_count INTEGER DEFAULT 1,
    order_status TEXT DEFAULT 'open',  -- open, completed, cancelled
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    total_amount REAL DEFAULT 0,
    payment_method TEXT,
    payment_status TEXT DEFAULT 'unpaid',  -- unpaid, paid
    notes TEXT,
    FOREIGN KEY (table_id) REFERENCES restaurant_tables (table_id),
    FOREIGN KEY (staff_id) REFERENCES staff (staff_id)
);

-- Order Items (Order Details)
CREATE TABLE order_items (
    order_item_id INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id INTEGER,
    item_id INTEGER,
    quantity INTEGER NOT NULL,
    item_price REAL NOT NULL,  -- Price at the time of order
    item_status TEXT DEFAULT 'pending',  -- pending, preparing, served, cancelled
    notes TEXT,
    FOREIGN KEY (order_id) REFERENCES orders (order_id),
    FOREIGN KEY (item_id) REFERENCES menu_items (item_id)
);

-- Daily Cash Register Tracking
CREATE TABLE cash_register (
    register_id INTEGER PRIMARY KEY AUTOINCREMENT,
    staff_id INTEGER,
    opening_balance REAL NOT NULL,
    closing_balance REAL,
    status TEXT DEFAULT 'open',  -- open, closed
    opened_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    closed_at TIMESTAMP,
    notes TEXT,
    FOREIGN KEY (staff_id) REFERENCES staff (staff_id)
);

-- Transaction History
CREATE TABLE transactions (
    transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id INTEGER,
    amount REAL NOT NULL,
    payment_method TEXT NOT NULL,
    transaction_type TEXT NOT NULL,  -- payment, refund
    staff_id INTEGER,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    notes TEXT,
    FOREIGN KEY (order_id) REFERENCES orders (order_id),
    FOREIGN KEY (staff_id) REFERENCES staff (staff_id)
);

-- Inventory Categories
CREATE TABLE inventory_categories (
    category_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    description TEXT
);

-- Inventory Items
CREATE TABLE inventory_items (
    inventory_id INTEGER PRIMARY KEY AUTOINCREMENT,
    category_id INTEGER,
    name TEXT NOT NULL,
    unit TEXT NOT NULL,  -- kg, l, pcs, etc.
    quantity_in_stock REAL DEFAULT 0,
    reorder_level REAL DEFAULT 0,
    unit_cost REAL DEFAULT 0,
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (category_id) REFERENCES inventory_categories (category_id)
);

-- Create indexes for better performance
CREATE INDEX idx_order_items_order_id ON order_items (order_id);
CREATE INDEX idx_orders_table_id ON orders (table_id);
CREATE INDEX idx_orders_staff_id ON orders (staff_id);
CREATE INDEX idx_menu_items_category_id ON menu_items (category_id);

-- Insert some initial sample data for testing
-- Default admin user
INSERT INTO staff (username, password, name, role, contact) 
VALUES ('admin', 'admin123', 'System Administrator', 'Manager', '555-1234');

-- Sample menu categories
INSERT INTO menu_categories (name, description) VALUES 
('Appetizers', 'Starters and small plates'),
('Main Courses', 'Primary dishes'),
('Desserts', 'Sweet treats'),
('Beverages', 'Drinks and refreshments');

-- Sample menu items
INSERT INTO menu_items (category_id, name, description, price) VALUES
(1, 'Garlic Bread', 'Toasted bread with garlic butter', 4.99),
(1, 'Mozzarella Sticks', 'Breaded and fried mozzarella with marinara sauce', 6.99),
(2, 'Spaghetti Carbonara', 'Classic pasta with egg, cheese, pancetta and black pepper', 12.99),
(2, 'Grilled Salmon', 'Salmon fillet with lemon herb sauce and vegetables', 16.99),
(3, 'Tiramisu', 'Coffee flavored Italian dessert', 5.99),
(4, 'Soda', 'Assorted soft drinks', 1.99),
(4, 'Coffee', 'Freshly brewed coffee', 2.49);

-- Sample restaurant tables
INSERT INTO restaurant_tables (table_number, capacity) VALUES
('A1', 2),
('A2', 2),
('B1', 4),
('B2', 4),
('C1', 6);